#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <glad/gl.h>

#include "render/shader.h"
#include "render/mesh.h"

typedef struct {
    SDL_Window* window;
    SDL_GLContext gl_context;
    int screen_width;
    int screen_height;

    // our current only object
    Shader triangle_shader;
    Mesh triangle_mesh;
} Renderer;

// LIFECYCLE //
int render_init(Renderer* renderer, int width, int height, const char* title);
void render_shutdown(Renderer* renderer);

// OPERATIONS //
void render_clear(Renderer* renderer, float r, float g, float b, float a, GLbitfield mask);
void render_present(Renderer* renderer);

// DRAWING //
int render_upload_triangle(Renderer* renderer);
void render_draw_triangle(Renderer* renderer);

#endif // RENDERER_H
