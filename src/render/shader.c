#include "shader.h"

#include <stdio.h>

// std140-ish layout WGSL enforces for uniform buffers
#define UNIFORM_SCALAR_OFFSET 0
#define UNIFORM_VECTOR_OFFSET 16
#define UNIFORM_BUFFER_SIZE 32

static WGPUShaderModule compile_wgsl(WGPUDevice device, const char* source)
{
    WGPUShaderSourceWGSL wgsl_source = { 0 };
    wgsl_source.chain.sType = WGPUSType_ShaderSourceWGSL;
    wgsl_source.code.data = source;
    wgsl_source.code.length = WGPU_STRLEN;

    WGPUShaderModuleDescriptor desc = { 0 };
    desc.nextInChain = &wgsl_source.chain;

    return wgpuDeviceCreateShaderModule(device, &desc);
}

sb_Shader shader_create(WGPUDevice device, WGPUQueue queue, WGPUTextureFormat surface_format, const char* vertex_src, const char* fragment_src)
{
    sb_Shader shader = { 0 };
    shader.device = device;
    shader.queue = queue;

    shader.vertex_module = compile_wgsl(device, vertex_src);
    if (!shader.vertex_module) {
        fprintf(stderr, "Failed to compile vertex shader module\n");
        return shader;
    }

    shader.fragment_module = compile_wgsl(device, fragment_src);
    if (!shader.fragment_module) {
        fprintf(stderr, "Failed to compile fragment shader module\n");
        wgpuShaderModuleRelease(shader.vertex_module);
        shader.vertex_module = NULL;
        return shader;
    }

    // Uniform buffer + bind group (single scalar + single vec3, see the offsets above)
    WGPUBindGroupLayoutEntry bgl_entry = { 0 };
    bgl_entry.binding = 0;
    bgl_entry.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    bgl_entry.buffer.type = WGPUBufferBindingType_Uniform;
    bgl_entry.buffer.minBindingSize = UNIFORM_BUFFER_SIZE;

    WGPUBindGroupLayoutDescriptor bgl_desc = { 0 };
    bgl_desc.entryCount = 1;
    bgl_desc.entries = &bgl_entry;

    shader.bind_group_layout = wgpuDeviceCreateBindGroupLayout(device, &bgl_desc);

    WGPUPipelineLayoutDescriptor pipeline_layout_desc = { 0 };
    pipeline_layout_desc.bindGroupLayoutCount = 1;
    pipeline_layout_desc.bindGroupLayouts = &shader.bind_group_layout;

    shader.pipeline_layout = wgpuDeviceCreatePipelineLayout(device, &pipeline_layout_desc);

    WGPUBufferDescriptor uniform_desc = { 0 };
    uniform_desc.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    uniform_desc.size = UNIFORM_BUFFER_SIZE;

    shader.uniform_buffer = wgpuDeviceCreateBuffer(device, &uniform_desc);

    WGPUBindGroupEntry bg_entry = { 0 };
    bg_entry.binding = 0;
    bg_entry.buffer = shader.uniform_buffer;
    bg_entry.offset = 0;
    bg_entry.size = UNIFORM_BUFFER_SIZE;

    WGPUBindGroupDescriptor bg_desc = { 0 };
    bg_desc.layout = shader.bind_group_layout;
    bg_desc.entryCount = 1;
    bg_desc.entries = &bg_entry;

    shader.bind_group = wgpuDeviceCreateBindGroup(device, &bg_desc);

    // Meshes only ever feed a single vec3 position at location 0 (see mesh.c)
    WGPUVertexAttribute position_attr = { 0 };
    position_attr.format = WGPUVertexFormat_Float32x3;
    position_attr.offset = 0;
    position_attr.shaderLocation = 0;

    WGPUVertexBufferLayout vertex_layout = { 0 };
    vertex_layout.stepMode = WGPUVertexStepMode_Vertex;
    vertex_layout.arrayStride = 3 * sizeof(float);
    vertex_layout.attributeCount = 1;
    vertex_layout.attributes = &position_attr;

    WGPUColorTargetState color_target = { 0 };
    color_target.format = surface_format;
    color_target.writeMask = WGPUColorWriteMask_All;

    WGPUFragmentState fragment_state = { 0 };
    fragment_state.module = shader.fragment_module;
    fragment_state.entryPoint.data = "fs_main";
    fragment_state.entryPoint.length = WGPU_STRLEN;
    fragment_state.targetCount = 1;
    fragment_state.targets = &color_target;

    WGPURenderPipelineDescriptor pipeline_desc = { 0 };
    pipeline_desc.layout = shader.pipeline_layout;
    pipeline_desc.vertex.module = shader.vertex_module;
    pipeline_desc.vertex.entryPoint.data = "vs_main";
    pipeline_desc.vertex.entryPoint.length = WGPU_STRLEN;
    pipeline_desc.vertex.bufferCount = 1;
    pipeline_desc.vertex.buffers = &vertex_layout;
    pipeline_desc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    pipeline_desc.multisample.count = 1;
    pipeline_desc.multisample.mask = 0xFFFFFFFF;
    pipeline_desc.fragment = &fragment_state;

    shader.pipeline = wgpuDeviceCreateRenderPipeline(device, &pipeline_desc);
    if (!shader.pipeline) {
        fprintf(stderr, "Failed to create render pipeline\n");
    }

    return shader;
}

void shader_destroy(sb_Shader* shader)
{
    if (shader->pipeline) {
        wgpuRenderPipelineRelease(shader->pipeline);
        shader->pipeline = NULL;
    }
    if (shader->bind_group) {
        wgpuBindGroupRelease(shader->bind_group);
        shader->bind_group = NULL;
    }
    if (shader->uniform_buffer) {
        wgpuBufferRelease(shader->uniform_buffer);
        shader->uniform_buffer = NULL;
    }
    if (shader->pipeline_layout) {
        wgpuPipelineLayoutRelease(shader->pipeline_layout);
        shader->pipeline_layout = NULL;
    }
    if (shader->bind_group_layout) {
        wgpuBindGroupLayoutRelease(shader->bind_group_layout);
        shader->bind_group_layout = NULL;
    }
    if (shader->fragment_module) {
        wgpuShaderModuleRelease(shader->fragment_module);
        shader->fragment_module = NULL;
    }
    if (shader->vertex_module) {
        wgpuShaderModuleRelease(shader->vertex_module);
        shader->vertex_module = NULL;
    }
}

void shader_use(const sb_Shader* shader, WGPURenderPassEncoder pass)
{
    wgpuRenderPassEncoderSetPipeline(pass, shader->pipeline);
    wgpuRenderPassEncoderSetBindGroup(pass, 0, shader->bind_group, 0, NULL);
}

void shader_set_float(const sb_Shader* shader, const char* name, float value)
{
    (void)name;
    wgpuQueueWriteBuffer(shader->queue, shader->uniform_buffer, UNIFORM_SCALAR_OFFSET, &value, sizeof(value));
}

void shader_set_vec3(const sb_Shader* shader, const char* name, float x, float y, float z)
{
    (void)name;
    float value[3] = { x, y, z };
    wgpuQueueWriteBuffer(shader->queue, shader->uniform_buffer, UNIFORM_VECTOR_OFFSET, value, sizeof(value));
}
