#include "render/engine.h"
#include "render/mesh.h"
#include "render/shader.h"
#include "world/object.h"
#include "world/scene.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(SDL_PLATFORM_MACOS)
#include <SDL3/SDL_metal.h>
#endif

// microui's built-in font+icon atlas: a single-channel 128x128 coverage
// texture (atlas_texture[]) plus a table of glyph/icon source rects (atlas[]).
// ATLAS_WHITE is a solid white patch, which is how plain flat-colored quads
// and textured glyphs share one pipeline below (see render_push_quad).
#include "atlas.inl"

#define ATLAS_TEXTURE_SIZE 128

// WGSL for the fixed-function pipeline that draws microui quads: samples the
// atlas texture's single coverage channel and multiplies it into the vertex
// color, so both flat rects (sampling the opaque ATLAS_WHITE patch) and
// glyphs/icons (sampling their actual coverage) go through the same path.
static const char* UI_QUAD_WGSL =
    "struct VertexInput {\n"
    "    @location(0) position: vec2<f32>,\n"
    "    @location(1) uv: vec2<f32>,\n"
    "    @location(2) color: vec4<f32>,\n"
    "};\n"
    "\n"
    "struct VertexOutput {\n"
    "    @builtin(position) clip_position: vec4<f32>,\n"
    "    @location(0) uv: vec2<f32>,\n"
    "    @location(1) color: vec4<f32>,\n"
    "};\n"
    "\n"
    "@group(0) @binding(0) var atlas_texture: texture_2d<f32>;\n"
    "@group(0) @binding(1) var atlas_sampler: sampler;\n"
    "\n"
    "@vertex\n"
    "fn vs_main(in: VertexInput) -> VertexOutput {\n"
    "    var out: VertexOutput;\n"
    "    out.clip_position = vec4<f32>(in.position, 0.0, 1.0);\n"
    "    out.uv = in.uv;\n"
    "    out.color = in.color;\n"
    "    return out;\n"
    "}\n"
    "\n"
    "@fragment\n"
    "fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {\n"
    "    let coverage = textureSample(atlas_texture, atlas_sampler, in.uv).r;\n"
    "    return vec4<f32>(in.color.rgb, in.color.a * coverage);\n"
    "}\n";

typedef struct {
    WGPUAdapter adapter;
} sb_AdapterRequest;

typedef struct {
    WGPUDevice device;
} sb_DeviceRequest;

// wgpu-native's native (non-browser) backend invokes request callbacks
// synchronously, inline, before wgpuInstanceRequestAdapter/wgpuAdapterRequestDevice
// even return -- there's no real async machinery to wait on here. (wgpuInstanceWaitAny
// itself is actually unimplemented in wgpu-native and panics if called, so it's not
// an option regardless.) The callback has already run by the time these return.

static void on_adapter_request(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void* userdata1, void* userdata2)
{
    (void)userdata2;
    sb_AdapterRequest* req = (sb_AdapterRequest*)userdata1;
    if (status == WGPURequestAdapterStatus_Success) {
        req->adapter = adapter;
    } else {
        fprintf(stderr, "Failed to request WebGPU adapter: %.*s\n", (int)message.length, message.data ? message.data : "");
    }
}

static void on_device_request(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void* userdata1, void* userdata2)
{
    (void)userdata2;
    sb_DeviceRequest* req = (sb_DeviceRequest*)userdata1;
    if (status == WGPURequestDeviceStatus_Success) {
        req->device = device;
    } else {
        fprintf(stderr, "Failed to request WebGPU device: %.*s\n", (int)message.length, message.data ? message.data : "");
    }
}

static void on_uncaptured_error(WGPUDevice const* device, WGPUErrorType type, WGPUStringView message, void* userdata1, void* userdata2)
{
    (void)device;
    (void)userdata1;
    (void)userdata2;
    fprintf(stderr, "WebGPU uncaptured error (type %d): %.*s\n", (int)type, (int)message.length, message.data ? message.data : "");
}

static void on_device_lost(WGPUDevice const* device, WGPUDeviceLostReason reason, WGPUStringView message, void* userdata1, void* userdata2)
{
    (void)device;
    (void)userdata1;
    (void)userdata2;
    fprintf(stderr, "WebGPU device lost (reason %d): %.*s\n", (int)reason, (int)message.length, message.data ? message.data : "");
}

// Wraps the SDL window's native handle in the platform-specific WGPUSurfaceSource
// wgpuInstanceCreateSurface() needs. SDL3 only exposes native handles via its
// window properties API, so which property names apply depends on the platform
// (and, on Linux, the active video driver).
static WGPUSurface create_surface(WGPUInstance instance, SDL_Window* window)
{
    SDL_PropertiesID props = SDL_GetWindowProperties(window);

#if defined(SDL_PLATFORM_WIN32)
    WGPUSurfaceSourceWindowsHWND source = { 0 };
    source.chain.sType = WGPUSType_SurfaceSourceWindowsHWND;
    source.hinstance = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_INSTANCE_POINTER, NULL);
    source.hwnd = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);

    WGPUSurfaceDescriptor desc = { 0 };
    desc.nextInChain = &source.chain;
    return wgpuInstanceCreateSurface(instance, &desc);
#elif defined(SDL_PLATFORM_MACOS)
    SDL_MetalView view = SDL_Metal_CreateView(window);

    WGPUSurfaceSourceMetalLayer source = { 0 };
    source.chain.sType = WGPUSType_SurfaceSourceMetalLayer;
    source.layer = SDL_Metal_GetLayer(view);

    WGPUSurfaceDescriptor desc = { 0 };
    desc.nextInChain = &source.chain;
    return wgpuInstanceCreateSurface(instance, &desc);
#else
    const char* driver = SDL_GetCurrentVideoDriver();

    if (driver && strcmp(driver, "wayland") == 0) {
        WGPUSurfaceSourceWaylandSurface source = { 0 };
        source.chain.sType = WGPUSType_SurfaceSourceWaylandSurface;
        source.display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        source.surface = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);

        WGPUSurfaceDescriptor desc = { 0 };
        desc.nextInChain = &source.chain;
        return wgpuInstanceCreateSurface(instance, &desc);
    }

    if (driver && strcmp(driver, "x11") == 0) {
        WGPUSurfaceSourceXlibWindow source = { 0 };
        source.chain.sType = WGPUSType_SurfaceSourceXlibWindow;
        source.display = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        source.window = (uint64_t)SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);

        WGPUSurfaceDescriptor desc = { 0 };
        desc.nextInChain = &source.chain;
        return wgpuInstanceCreateSurface(instance, &desc);
    }

    fprintf(stderr, "Unsupported SDL video driver for WebGPU surface creation: %s\n", driver ? driver : "(null)");
    return NULL;
#endif
}

static void configure_surface(sb_Renderer* renderer, uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0) {
        return;
    }

    WGPUSurfaceConfiguration config = { 0 };
    config.device = renderer->device;
    config.format = renderer->surface_format;
    config.usage = WGPUTextureUsage_RenderAttachment;
    config.width = width;
    config.height = height;
    config.presentMode = WGPUPresentMode_Fifo;
    config.alphaMode = WGPUCompositeAlphaMode_Auto;

    wgpuSurfaceConfigure(renderer->surface, &config);
}

static int create_ui_pipeline(sb_Renderer* renderer)
{
    WGPUShaderSourceWGSL wgsl_source = { 0 };
    wgsl_source.chain.sType = WGPUSType_ShaderSourceWGSL;
    wgsl_source.code.data = UI_QUAD_WGSL;
    wgsl_source.code.length = WGPU_STRLEN;

    WGPUShaderModuleDescriptor shader_desc = { 0 };
    shader_desc.nextInChain = &wgsl_source.chain;

    renderer->ui_shader = wgpuDeviceCreateShaderModule(renderer->device, &shader_desc);
    if (!renderer->ui_shader) {
        return 0;
    }

    WGPUVertexAttribute attributes[3] = { 0 };
    attributes[0].format = WGPUVertexFormat_Float32x2; // position
    attributes[0].offset = 0;
    attributes[0].shaderLocation = 0;
    attributes[1].format = WGPUVertexFormat_Float32x2; // uv
    attributes[1].offset = 2 * sizeof(float);
    attributes[1].shaderLocation = 1;
    attributes[2].format = WGPUVertexFormat_Float32x4; // color
    attributes[2].offset = 4 * sizeof(float);
    attributes[2].shaderLocation = 2;

    WGPUVertexBufferLayout vertex_layout = { 0 };
    vertex_layout.stepMode = WGPUVertexStepMode_Vertex;
    vertex_layout.arrayStride = 8 * sizeof(float);
    vertex_layout.attributeCount = 3;
    vertex_layout.attributes = attributes;

    WGPUBindGroupLayoutEntry bgl_entries[2] = { 0 };
    bgl_entries[0].binding = 0;
    bgl_entries[0].visibility = WGPUShaderStage_Fragment;
    bgl_entries[0].texture.sampleType = WGPUTextureSampleType_Float;
    bgl_entries[0].texture.viewDimension = WGPUTextureViewDimension_2D;
    bgl_entries[1].binding = 1;
    bgl_entries[1].visibility = WGPUShaderStage_Fragment;
    bgl_entries[1].sampler.type = WGPUSamplerBindingType_Filtering;

    WGPUBindGroupLayoutDescriptor bgl_desc = { 0 };
    bgl_desc.entryCount = 2;
    bgl_desc.entries = bgl_entries;

    renderer->ui_atlas_bind_group_layout = wgpuDeviceCreateBindGroupLayout(renderer->device, &bgl_desc);

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
    pipeline_layout_desc.bindGroupLayoutCount = 1;
    pipeline_layout_desc.bindGroupLayouts = &renderer->ui_atlas_bind_group_layout;

    WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(renderer->device, &pipeline_layout_desc);

    WGPUBlendComponent blend_color = { 0 };
    blend_color.operation = WGPUBlendOperation_Add;
    blend_color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blend_color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;

    WGPUBlendComponent blend_alpha = { 0 };
    blend_alpha.operation = WGPUBlendOperation_Add;
    blend_alpha.srcFactor = WGPUBlendFactor_One;
    blend_alpha.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;

    WGPUBlendState blend_state = { 0 };
    blend_state.color = blend_color;
    blend_state.alpha = blend_alpha;

    WGPUColorTargetState color_target = { 0 };
    color_target.format = renderer->surface_format;
    color_target.blend = &blend_state;
    color_target.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragment_state = { 0 };
    fragment_state.module = renderer->ui_shader;
    fragment_state.entryPoint.data = "fs_main";
    fragment_state.entryPoint.length = WGPU_STRLEN;
    fragment_state.targetCount = 1;
    fragment_state.targets = &color_target;

    WGPURenderPipelineDescriptor pipeline_desc = { 0 };
    pipeline_desc.layout = pipeline_layout;
    pipeline_desc.vertex.module = renderer->ui_shader;
    pipeline_desc.vertex.entryPoint.data = "vs_main";
    pipeline_desc.vertex.entryPoint.length = WGPU_STRLEN;
    pipeline_desc.vertex.bufferCount = 1;
    pipeline_desc.vertex.buffers = &vertex_layout;
    pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = 0xFFFFFFFF;
    pipeline_desc.fragment = &fragment_state;

    renderer->ui_pipeline = wgpuDeviceCreateRenderPipeline(renderer->device, &pipeline_desc);
    wgpuPipelineLayoutRelease(pipeline_layout);
    if (!renderer->ui_pipeline) {
        return 0;
    }

    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    buffer_desc.size = sizeof(renderer->ui_quad_vertices);

    renderer->ui_vertex_buffer = wgpuDeviceCreateBuffer(renderer->device, &buffer_desc);
    if (!renderer->ui_vertex_buffer) {
        return 0;
    }

    // Upload microui's font+icon atlas (see atlas.inl) as a single-channel coverage texture.
    WGPUTextureDescriptor texture_desc = { 0 };
    texture_desc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    texture_desc.dimension = WGPUTextureDimension_2D;
    texture_desc.size.width = ATLAS_TEXTURE_SIZE;
    texture_desc.size.height = ATLAS_TEXTURE_SIZE;
    texture_desc.size.depthOrArrayLayers = 1;
    texture_desc.format = WGPUTextureFormat_R8Unorm;
    texture_desc.mipLevelCount = 1;
    texture_desc.sampleCount = 1;

    renderer->ui_atlas_texture = wgpuDeviceCreateTexture(renderer->device, &texture_desc);
    if (!renderer->ui_atlas_texture) {
        return 0;
    }

    WGPUTexelCopyTextureInfo copy_dst = { 0 };
    copy_dst.texture = renderer->ui_atlas_texture;

    WGPUTexelCopyBufferLayout copy_layout = { 0 };
    copy_layout.bytesPerRow = ATLAS_TEXTURE_SIZE;
    copy_layout.rowsPerImage = ATLAS_TEXTURE_SIZE;

    WGPUExtent3D copy_size = { ATLAS_TEXTURE_SIZE, ATLAS_TEXTURE_SIZE, 1 };

    wgpuQueueWriteTexture(renderer->queue, &copy_dst, atlas_texture, sizeof(atlas_texture), &copy_layout, &copy_size);

    WGPUTextureViewDescriptor view_desc = { 0 };
    view_desc.format = WGPUTextureFormat_R8Unorm;
    view_desc.dimension = WGPUTextureViewDimension_2D;
    view_desc.mipLevelCount = 1;
    view_desc.arrayLayerCount = 1;

    renderer->ui_atlas_view = wgpuTextureCreateView(renderer->ui_atlas_texture, &view_desc);

    WGPUSamplerDescriptor sampler_desc = { 0 };
    sampler_desc.addressModeU = WGPUAddressMode_ClampToEdge;
    sampler_desc.addressModeV = WGPUAddressMode_ClampToEdge;
    sampler_desc.addressModeW = WGPUAddressMode_ClampToEdge;
    sampler_desc.magFilter = WGPUFilterMode_Nearest;
    sampler_desc.minFilter = WGPUFilterMode_Nearest;
    sampler_desc.mipmapFilter = WGPUMipmapFilterMode_Nearest;
    sampler_desc.lodMinClamp = 0.0f;
    sampler_desc.lodMaxClamp = 32.0f;
    sampler_desc.maxAnisotropy = 1;

    renderer->ui_atlas_sampler = wgpuDeviceCreateSampler(renderer->device, &sampler_desc);

    WGPUBindGroupEntry bg_entries[2] = { 0 };
    bg_entries[0].binding = 0;
    bg_entries[0].textureView = renderer->ui_atlas_view;
    bg_entries[1].binding = 1;
    bg_entries[1].sampler = renderer->ui_atlas_sampler;

    WGPUBindGroupDescriptor bg_desc = { 0 };
    bg_desc.layout = renderer->ui_atlas_bind_group_layout;
    bg_desc.entryCount = 2;
    bg_desc.entries = bg_entries;

    renderer->ui_atlas_bind_group = wgpuDeviceCreateBindGroup(renderer->device, &bg_desc);

    return renderer->ui_atlas_bind_group != NULL;
}

int render_init(sb_Renderer* renderer, int width, int height, const char* title)
{
    memset(renderer, 0, sizeof(*renderer));
    renderer->screen_width = (uint16_t)width;
    renderer->screen_height = (uint16_t)height;

    renderer->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
    if (!renderer->window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    WGPUInstanceDescriptor instance_desc = { 0 };
    renderer->instance = wgpuCreateInstance(&instance_desc);
    if (!renderer->instance) {
        fprintf(stderr, "Failed to create WebGPU instance\n");
        render_shutdown(renderer);
        return 0;
    }

    renderer->surface = create_surface(renderer->instance, renderer->window);
    if (!renderer->surface) {
        fprintf(stderr, "Failed to create WebGPU surface\n");
        render_shutdown(renderer);
        return 0;
    }

    WGPURequestAdapterOptions adapter_opts = { 0 };
    adapter_opts.compatibleSurface = renderer->surface;
    adapter_opts.powerPreference = WGPUPowerPreference_HighPerformance;

    sb_AdapterRequest adapter_req = { 0 };
    WGPURequestAdapterCallbackInfo adapter_cb = { 0 };
    adapter_cb.mode = WGPUCallbackMode_WaitAnyOnly;
    adapter_cb.callback = on_adapter_request;
    adapter_cb.userdata1 = &adapter_req;

    wgpuInstanceRequestAdapter(renderer->instance, &adapter_opts, adapter_cb);

    if (!adapter_req.adapter) {
        fprintf(stderr, "Failed to acquire a WebGPU adapter\n");
        render_shutdown(renderer);
        return 0;
    }
    renderer->adapter = adapter_req.adapter;

    WGPUDeviceDescriptor device_desc = { 0 };
    device_desc.uncapturedErrorCallbackInfo.callback = on_uncaptured_error;
    device_desc.deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowSpontaneous;
    device_desc.deviceLostCallbackInfo.callback = on_device_lost;

    sb_DeviceRequest device_req = { 0 };
    WGPURequestDeviceCallbackInfo device_cb = { 0 };
    device_cb.mode = WGPUCallbackMode_WaitAnyOnly;
    device_cb.callback = on_device_request;
    device_cb.userdata1 = &device_req;

    wgpuAdapterRequestDevice(renderer->adapter, &device_desc, device_cb);

    if (!device_req.device) {
        fprintf(stderr, "Failed to acquire a WebGPU device\n");
        render_shutdown(renderer);
        return 0;
    }
    renderer->device = device_req.device;
    renderer->queue = wgpuDeviceGetQueue(renderer->device);

    WGPUSurfaceCapabilities caps = { 0 };
    if (wgpuSurfaceGetCapabilities(renderer->surface, renderer->adapter, &caps) != WGPUStatus_Success || caps.formatCount == 0) {
        fprintf(stderr, "Failed to query WebGPU surface capabilities\n");
        render_shutdown(renderer);
        return 0;
    }
    renderer->surface_format = caps.formats[0];
    wgpuSurfaceCapabilitiesFreeMembers(caps);

    configure_surface(renderer, (uint32_t)width, (uint32_t)height);

    if (!create_ui_pipeline(renderer)) {
        fprintf(stderr, "Failed to create UI quad pipeline\n");
        render_shutdown(renderer);
        return 0;
    }

    printf("WebGPU initialized successfully! Surface format: %d\n", (int)renderer->surface_format);

    return 1;
}

void render_shutdown(sb_Renderer* renderer)
{
    if (renderer->ui_atlas_bind_group) {
        wgpuBindGroupRelease(renderer->ui_atlas_bind_group);
        renderer->ui_atlas_bind_group = NULL;
    }

    if (renderer->ui_atlas_bind_group_layout) {
        wgpuBindGroupLayoutRelease(renderer->ui_atlas_bind_group_layout);
        renderer->ui_atlas_bind_group_layout = NULL;
    }

    if (renderer->ui_atlas_sampler) {
        wgpuSamplerRelease(renderer->ui_atlas_sampler);
        renderer->ui_atlas_sampler = NULL;
    }

    if (renderer->ui_atlas_view) {
        wgpuTextureViewRelease(renderer->ui_atlas_view);
        renderer->ui_atlas_view = NULL;
    }

    if (renderer->ui_atlas_texture) {
        wgpuTextureRelease(renderer->ui_atlas_texture);
        renderer->ui_atlas_texture = NULL;
    }

    if (renderer->ui_vertex_buffer) {
        wgpuBufferRelease(renderer->ui_vertex_buffer);
        renderer->ui_vertex_buffer = NULL;
    }

    if (renderer->ui_pipeline) {
        wgpuRenderPipelineRelease(renderer->ui_pipeline);
        renderer->ui_pipeline = NULL;
    }

    if (renderer->ui_shader) {
        wgpuShaderModuleRelease(renderer->ui_shader);
        renderer->ui_shader = NULL;
    }

    if (renderer->queue) {
        wgpuQueueRelease(renderer->queue);
        renderer->queue = NULL;
    }

    if (renderer->device) {
        wgpuDeviceRelease(renderer->device);
        renderer->device = NULL;
    }

    if (renderer->adapter) {
        wgpuAdapterRelease(renderer->adapter);
        renderer->adapter = NULL;
    }

    if (renderer->surface) {
        wgpuSurfaceRelease(renderer->surface);
        renderer->surface = NULL;
    }

    if (renderer->instance) {
        wgpuInstanceRelease(renderer->instance);
        renderer->instance = NULL;
    }

    if (renderer->window) {
        SDL_DestroyWindow(renderer->window);
        renderer->window = NULL;
    }

    SDL_Quit();
}

void render_resize(sb_Renderer* renderer, SDL_WindowEvent* event)
{
    renderer->screen_width = (uint16_t)event->data1;
    renderer->screen_height = (uint16_t)event->data2;
    configure_surface(renderer, renderer->screen_width, renderer->screen_height);
}

// Draws every quad accumulated by render_push_quad() since the last flush as
// one batched draw call. This is NOT an optimization -- it's required for
// correctness. wgpuQueueWriteBuffer() executes as soon as it's called, but
// SetVertexBuffer/Draw are only *recorded*; they don't actually run on the
// GPU until the command buffer built this frame is submitted in
// render_present(). If render_push_quad() wrote+drew one quad at a time into
// a single shared buffer, every one of those writes would happen (in queue
// order) before any of the draws do (since they're all in one command buffer
// submitted once, at the end of the frame) -- so every draw would end up
// reading whatever the *last* write left in the buffer, i.e. every quad that
// frame would render as a copy of the last one. Batching sidesteps this: one
// write covers everything that will be drawn from it, before any of the
// matching draws run.
static void flush_ui_quads(sb_Renderer* renderer)
{
    if (renderer->ui_quad_count == 0 || !renderer->frame_pass) {
        return;
    }

    size_t vertex_count = (size_t)renderer->ui_quad_count * 6;
    size_t bytes = vertex_count * sizeof(renderer->ui_quad_vertices[0]);

    wgpuQueueWriteBuffer(renderer->queue, renderer->ui_vertex_buffer, 0, renderer->ui_quad_vertices, bytes);

    wgpuRenderPassEncoderSetPipeline(renderer->frame_pass, renderer->ui_pipeline);
    wgpuRenderPassEncoderSetBindGroup(renderer->frame_pass, 0, renderer->ui_atlas_bind_group, 0, NULL);
    wgpuRenderPassEncoderSetVertexBuffer(renderer->frame_pass, 0, renderer->ui_vertex_buffer, 0, bytes);
    wgpuRenderPassEncoderDraw(renderer->frame_pass, (uint32_t)vertex_count, 1, 0, 0);

    renderer->ui_quad_count = 0;
}

void render_begin_frame(sb_Renderer* renderer, float r, float g, float b, float a)
{
    renderer->ui_quad_count = 0;

    WGPUSurfaceTexture surface_texture = { 0 };
    wgpuSurfaceGetCurrentTexture(renderer->surface, &surface_texture);

    if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
        surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal) {
        // Surface is outdated/lost (e.g. mid-resize); reconfigure and pick it back up next frame.
        configure_surface(renderer, renderer->screen_width, renderer->screen_height);
        return;
    }

    renderer->frame_texture = surface_texture.texture;
    renderer->frame_view = wgpuTextureCreateView(renderer->frame_texture, NULL);

    WGPUCommandEncoderDescriptor encoder_desc = { 0 };
    renderer->frame_encoder = wgpuDeviceCreateCommandEncoder(renderer->device, &encoder_desc);

    WGPURenderPassColorAttachment color_attachment = { 0 };
    color_attachment.view = renderer->frame_view;
    color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
    color_attachment.loadOp = WGPULoadOp_Clear;
    color_attachment.storeOp = WGPUStoreOp_Store;
    color_attachment.clearValue.r = r;
    color_attachment.clearValue.g = g;
    color_attachment.clearValue.b = b;
    color_attachment.clearValue.a = a;

    WGPURenderPassDescriptor pass_desc = { 0 };
    pass_desc.colorAttachmentCount = 1;
    pass_desc.colorAttachments = &color_attachment;

    renderer->frame_pass = wgpuCommandEncoderBeginRenderPass(renderer->frame_encoder, &pass_desc);
}

void render_present(sb_Renderer* renderer)
{
    if (!renderer->frame_pass) {
        return;
    }

    flush_ui_quads(renderer);

    wgpuRenderPassEncoderEnd(renderer->frame_pass);
    wgpuRenderPassEncoderRelease(renderer->frame_pass);
    renderer->frame_pass = NULL;

    WGPUCommandBufferDescriptor cmd_buffer_desc = { 0 };
    WGPUCommandBuffer cmd_buffer = wgpuCommandEncoderFinish(renderer->frame_encoder, &cmd_buffer_desc);
    wgpuCommandEncoderRelease(renderer->frame_encoder);
    renderer->frame_encoder = NULL;

    wgpuQueueSubmit(renderer->queue, 1, &cmd_buffer);
    wgpuCommandBufferRelease(cmd_buffer);

    wgpuSurfacePresent(renderer->surface);

    wgpuTextureViewRelease(renderer->frame_view);
    renderer->frame_view = NULL;
    wgpuTextureRelease(renderer->frame_texture);
    renderer->frame_texture = NULL;
}

void render_draw(sb_Renderer* renderer, sb_Scene* scene)
{
    if (!renderer->frame_pass) {
        return;
    }

    // currently we draw in the sense that use the shader and draw the mesh
    for (size_t i = 0; i < scene->object_count; i++) {
        sb_Object* obj = &scene->objects[i];
        shader_use(&obj->shader, renderer->frame_pass);
        mesh_draw(&obj->mesh, renderer->frame_pass);
    }
}

mu_Rect render_atlas_rect(int id)
{
    return atlas[id];
}

void render_push_quad(sb_Renderer* renderer, mu_Rect dst, mu_Rect src, mu_Color color)
{
    if (!renderer->frame_pass || renderer->screen_width == 0 || renderer->screen_height == 0) {
        return;
    }

    if (renderer->ui_quad_count >= SB_MAX_UI_QUADS) {
        flush_ui_quads(renderer);
    }

    float x0 = (float)dst.x / (float)renderer->screen_width * 2.0f - 1.0f;
    float x1 = (float)(dst.x + dst.w) / (float)renderer->screen_width * 2.0f - 1.0f;
    float y0 = 1.0f - (float)dst.y / (float)renderer->screen_height * 2.0f;
    float y1 = 1.0f - (float)(dst.y + dst.h) / (float)renderer->screen_height * 2.0f;

    float u0 = (float)src.x / (float)ATLAS_TEXTURE_SIZE;
    float u1 = (float)(src.x + src.w) / (float)ATLAS_TEXTURE_SIZE;
    float v0 = (float)src.y / (float)ATLAS_TEXTURE_SIZE;
    float v1 = (float)(src.y + src.h) / (float)ATLAS_TEXTURE_SIZE;

    float cr = color.r / 255.0f;
    float cg = color.g / 255.0f;
    float cb = color.b / 255.0f;
    float ca = color.a / 255.0f;

    float vertices[6][8] = {
        { x0, y0, u0, v0, cr, cg, cb, ca },
        { x1, y0, u1, v0, cr, cg, cb, ca },
        { x0, y1, u0, v1, cr, cg, cb, ca },
        { x0, y1, u0, v1, cr, cg, cb, ca },
        { x1, y0, u1, v0, cr, cg, cb, ca },
        { x1, y1, u1, v1, cr, cg, cb, ca },
    };

    memcpy(&renderer->ui_quad_vertices[renderer->ui_quad_count * 6], vertices, sizeof(vertices));
    renderer->ui_quad_count++;
}

void render_set_scissor(sb_Renderer* renderer, mu_Rect rect)
{
    if (!renderer->frame_pass) {
        return;
    }

    // Quads accumulated so far were laid out assuming the scissor rect that
    // was active when they were pushed; flush them before it changes.
    flush_ui_quads(renderer);

    int x = rect.x < 0 ? 0 : rect.x;
    int y = rect.y < 0 ? 0 : rect.y;
    int w = rect.w < 0 ? 0 : rect.w;
    int h = rect.h < 0 ? 0 : rect.h;

    if (x > renderer->screen_width) {
        x = renderer->screen_width;
    }
    if (y > renderer->screen_height) {
        y = renderer->screen_height;
    }
    if (x + w > renderer->screen_width) {
        w = renderer->screen_width - x;
    }
    if (y + h > renderer->screen_height) {
        h = renderer->screen_height - y;
    }

    wgpuRenderPassEncoderSetScissorRect(renderer->frame_pass, (uint32_t)x, (uint32_t)y, (uint32_t)w, (uint32_t)h);
}
