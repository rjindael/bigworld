#include "render/engine.h"
#include "world/object.h"
#include "world/scene.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int render_init(sb_Renderer* renderer, int width, int height, const char* title)
{
    renderer->screen_width = width;
    renderer->screen_height = height;

    // Set OpenGL attributes (core profile @ version 3.3)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Create SDL window
    renderer->window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!renderer->window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    // Create SDL OpenGL context
    renderer->gl_context = SDL_GL_CreateContext(renderer->window);
    if (!renderer->gl_context) {
        fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
        SDL_DestroyWindow(renderer->window);
        SDL_Quit();
        return 0;
    }

    // Make the GL context current
    SDL_GL_MakeCurrent(renderer->window, renderer->gl_context);

    // and then load OpenGL functions with GLAD
    int glad_version = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    if (!glad_version) {
        fprintf(stderr, "Failed to load OpenGL functions with GLAD\n");
        SDL_GL_DestroyContext(renderer->gl_context);
        SDL_DestroyWindow(renderer->window);
        SDL_Quit();
        return 0;
    }

    printf("OpenGL loaded successfully! OpenGL version: %d.%d\n", GLAD_VERSION_MAJOR(glad_version), GLAD_VERSION_MINOR(glad_version));

    // Set viewport
    glViewport(0, 0, width, height);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    return 1;
}

void render_shutdown(sb_Renderer* renderer)
{
    if (renderer->gl_context) {
        SDL_GL_DestroyContext(renderer->gl_context);
    }

    if (renderer->window) {
        SDL_DestroyWindow(renderer->window);
    }

    SDL_Quit();
}

void render_resize(sb_Renderer* renderer, SDL_WindowEvent* event)
{
    renderer->screen_width = event->data1;
    renderer->screen_height = event->data2;
    glViewport(0, 0, renderer->screen_width, renderer->screen_height);
}

void render_clear(sb_Renderer* renderer, float r, float g, float b, float a, GLbitfield mask)
{
    glClearColor(r, g, b, a);
    glClear(mask);
}

void render_present(sb_Renderer* renderer)
{
    SDL_GL_SwapWindow(renderer->window);
}

void render_draw(sb_Renderer* renderer, sb_Scene* scene)
{
    // currently we draw in the sense that use the shader and draw the mesh
    for (size_t i = 0; i < scene->object_count; i++) {
        sb_Object* obj = &scene->objects[i];
        shader_use(&obj->shader);
        mesh_draw(&obj->mesh);
    }
}