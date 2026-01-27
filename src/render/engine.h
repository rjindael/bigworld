#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <glad/gl.h>
#include "gfx/color.h"

typedef struct sb_Renderer {
    SDL_Window* window;
    SDL_GLContext gl_context;
    uint16_t screen_width;
    uint16_t screen_height;
} sb_Renderer;

typedef struct sb_Scene sb_Scene;

////////////////// LIFECYCLE //////////////////

int render_init(sb_Renderer* renderer, int width, int height, const char* title);
void render_shutdown(sb_Renderer* renderer);

////////////////// OPERATIONS //////////////////

void render_resize(sb_Renderer* renderer, SDL_WindowEvent* event);
void render_clear(sb_Renderer* renderer, float r, float g, float b, float a, GLbitfield mask);
void render_present(sb_Renderer* renderer);

////////////////// DRAWING //////////////////

void render_draw(sb_Renderer* renderer, sb_Scene* scene);

// UI
//void render_push_quad(mu_Rect rect, mu_Rect src, sb_Color4 color);

#endif // RENDERER_H
