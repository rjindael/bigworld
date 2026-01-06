#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL.h>
#include <glad/gl.h>

// fwd declaration - Scene is defined in world/scene.h
struct Scene;

typedef struct {
    SDL_Window* window;
    SDL_GLContext gl_context;
    int screen_width;
    int screen_height;
} Renderer;

////////////////// LIFECYCLE //////////////////

bool render_init(Renderer* renderer, int width, int height, const char* title);
void render_shutdown(Renderer* renderer);

////////////////// OPERATIONS //////////////////

void render_clear(Renderer* renderer, float r, float g, float b, float a, GLbitfield mask);
void render_present(Renderer* renderer);

////////////////// DRAWING //////////////////

void render_draw(Renderer* renderer, struct Scene* scene);

#endif // RENDERER_H
