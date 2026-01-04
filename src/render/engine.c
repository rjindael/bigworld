#include "render/engine.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int render_init(Renderer* renderer, int width, int height, const char* title)
{
    renderer->screen_width = width;
    renderer->screen_height = height;

    // Initialize SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 0;
    }

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

    if (!render_upload_triangle(renderer)) {
        fprintf(stderr, "Failed to upload triangle data\n");
        render_shutdown(renderer);
        return 0;
    }

    // the triangle we created & uploaded will be drawn in the render loop (shambhala_update)
    // probably should abstract how objects are collated and drawn later

    return 1;
}

void render_shutdown(Renderer* renderer)
{
    mesh_destroy(&renderer->triangle_mesh);
    shader_destroy(&renderer->triangle_shader);

    if (renderer->gl_context) {
        SDL_GL_DestroyContext(renderer->gl_context);
    }

    if (renderer->window) {
        SDL_DestroyWindow(renderer->window);
    }

    SDL_Quit();
}

void render_clear(Renderer* renderer, float r, float g, float b, float a, GLbitfield mask)
{
    glClearColor(r, g, b, a);
    glClear(mask);
}

void render_present(Renderer* renderer)
{
    SDL_GL_SwapWindow(renderer->window);
}

// example of how we'd create a new obj for the renderer to draw
int render_upload_triangle(Renderer* renderer)
{
    // first need to make it's shader
    char* vertex_src = io_read_file("shaders/vert/tri.glsl");
    char* fragment_src = io_read_file("shaders/frag/tri.glsl");

    if (!vertex_src || !fragment_src) {
        fprintf(stderr, "Failed to load shader files\n");
        free(vertex_src);
        free(fragment_src);
        return 0;
    }

    renderer->triangle_shader = shader_create(vertex_src, fragment_src);

    free(vertex_src);
    free(fragment_src);

    if (renderer->triangle_shader.program_id == 0) {
        fprintf(stderr, "Failed to create shader program\n");
        return 0;
    }

    // then create the mesh
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f,  -0.5f, 0.0f, // right
        0.0f,  0.5f,  0.0f // top
    };

    renderer->triangle_mesh = mesh_create(vertices, 9, NULL, 0);

    return 1;
}

void render_draw_triangle(Renderer* renderer)
{
    shader_use(&renderer->triangle_shader);
    mesh_draw(&renderer->triangle_mesh);
}
