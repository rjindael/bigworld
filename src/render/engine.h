#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <webgpu/webgpu.h>
#include <microui.h>
#include "gfx/color.h"

// Cap on how many UI quads (one per rect fill / glyph / icon)
#define SB_MAX_UI_QUADS 4096

typedef struct sb_Renderer {
    SDL_Window* window;

    WGPUInstance instance;
    WGPUSurface surface;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUTextureFormat surface_format;

    // fixed-function pipeline for textured/flat-colored UI quads (microui),
    // sampling microui's built-in font+icon atlas (see engine.c / atlas.inl)
    WGPUShaderModule ui_shader;
    WGPURenderPipeline ui_pipeline;
    WGPUBuffer ui_vertex_buffer;
    WGPUTexture ui_atlas_texture;
    WGPUTextureView ui_atlas_view;
    WGPUSampler ui_atlas_sampler;
    WGPUBindGroupLayout ui_atlas_bind_group_layout;
    WGPUBindGroup ui_atlas_bind_group;

    // Quads accumulated by render_push_quad() this frame
    float ui_quad_vertices[SB_MAX_UI_QUADS * 6][8]; // position(2) + uv(2) + color(4)
    int ui_quad_count;

    uint16_t screen_width;
    uint16_t screen_height;

    // per-frame state, only valid between render_begin_frame() and render_present()
    WGPUTexture frame_texture;
    WGPUTextureView frame_view;
    WGPUCommandEncoder frame_encoder;
    WGPURenderPassEncoder frame_pass;
} sb_Renderer;

typedef struct sb_Scene sb_Scene;

////////////////// LIFECYCLE //////////////////

int render_init(sb_Renderer* renderer, int width, int height, const char* title);
void render_shutdown(sb_Renderer* renderer);

////////////////// OPERATIONS //////////////////

void render_resize(sb_Renderer* renderer, SDL_WindowEvent* event);
void render_begin_frame(sb_Renderer* renderer, float r, float g, float b, float a);
void render_present(sb_Renderer* renderer);

////////////////// DRAWING //////////////////

void render_draw(sb_Renderer* renderer, sb_Scene* scene);

// UI
enum { RENDER_ATLAS_WHITE = MU_ICON_MAX, RENDER_ATLAS_FONT };

mu_Rect render_atlas_rect(int id);
void render_push_quad(sb_Renderer* renderer, mu_Rect dst, mu_Rect src, mu_Color color);
void render_set_scissor(sb_Renderer* renderer, mu_Rect rect);

#endif // RENDERER_H
