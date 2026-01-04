#include "shambhala.h"
#include <SDL3/SDL.h>
#include <glad/gl.h>
#include <stdio.h>

int shambhala_init(Shambhala* app, int width, int height, const char* title)
{
    app->running = false;

    if (!render_init(&app->renderer, width, height, title)) {
        fprintf(stderr, "Failed to initialize renderer\n");
        return 0;
    }

    app->running = true;
    return 1;
}

void shambhala_shutdown(Shambhala* app)
{
    render_shutdown(&app->renderer);
    app->running = false;
}

void shambhala_process_events(Shambhala* app)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            app->running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            if (event.key.key == SDLK_ESCAPE) {
                app->running = false;
            }
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            app->renderer.screen_width = event.window.data1;
            app->renderer.screen_height = event.window.data2;
            glViewport(0, 0, app->renderer.screen_width, app->renderer.screen_height);
            break;
        }
    }
}

// WHERE IT ALL HAPPENS!!!
void shambhala_update(Shambhala* app)
{
    // Clear screen first, then draw
    render_clear(&app->renderer, 0.2f, 0.3f, 0.3f, 1.0f, GL_COLOR_BUFFER_BIT);
    render_draw_triangle(&app->renderer);
}

void shambhala_render(Shambhala* app)
{
    render_present(&app->renderer);
}

void shambhala_run(Shambhala* app)
{
    while (app->running) {
        shambhala_process_events(app);
        shambhala_update(app);
        shambhala_render(app);
    }
}
