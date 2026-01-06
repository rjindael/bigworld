#include "shambhala.h"
#include "input.h"

#include <stdio.h>
#include <time.h>
#include <glad/gl.h>
#include <SDL3/SDL.h>

int shambhala_init(Shambhala* app, int width, int height, const char* title)
{
    app->running = false;

    // This is needed for psuedorandom data
    srand(time(NULL));

    if (!render_init(&app->renderer, width, height, title)) {
        fprintf(stderr, "Failed to initialize renderer\n");
        return 0;
    }

    app->scene = scene_create();
    app->running = true;

    return 1;
}

void shambhala_shutdown(Shambhala* app)
{
    render_shutdown(&app->renderer);
    scene_destroy(&app->scene);

    app->running = false;
}

// WHERE IT ALL HAPPENS!!!
void shambhala_update(Shambhala* app)
{
    // Clear screen first, update scene, and then draw the scene.
    render_clear(&app->renderer, 1.0f, 1.0f, 1.0f, 1.0f, GL_COLOR_BUFFER_BIT);
    scene_update(&app->scene);
    render_draw(&app->renderer, &app->scene);
}

void shambhala_render(Shambhala* app)
{
    render_present(&app->renderer);
}

void shambhala_run(Shambhala* app)
{
    while (app->running) {
        // note that input can signal events to the scene itself
        // e.g., moving objects, camera, etc.
        // those events get processed by scene_update
        input_process(app);

        // update & render
        shambhala_update(app);
        shambhala_render(app);
    }
}
