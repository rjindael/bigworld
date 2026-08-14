/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "shambhala.h"
#include "render/engine.h"
#include "world/scene.h"
#include "input.h"
#include "ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL3/SDL.h>

int shambhala_init(sb_App* app, int width, int height, const char* title)
{
    memset(app, 0, sizeof(sb_App));

    app->running = 0;

    // This is needed for psuedorandom data
    srand(time(NULL));

    app->renderer = malloc(sizeof(sb_Renderer));
    if (!app->renderer) {
        fprintf(stderr, "Failed to allocate renderer\n");
        return 0;
    }

    if (!render_init(app->renderer, width, height, title)) {
        fprintf(stderr, "Failed to initialize renderer\n");
        free(app->renderer);
        return 0;
    }

    app->scene = malloc(sizeof(sb_Scene));
    if (!app->scene) {
        fprintf(stderr, "Failed to allocate scene\n");
        render_shutdown(app->renderer);
        free(app->renderer);
        return 0;
    }

    *app->scene = scene_create(app->renderer);

    app->ui = malloc(sizeof(sb_UI));
    if (!app->ui) {
        fprintf(stderr, "Failed to allocate UI\n");
        scene_destroy(app->scene);
        free(app->scene);
        render_shutdown(app->renderer);
        free(app->renderer);
        return 0;
    }

    ui_init(app->ui);
    app->running = 1;

    return 1;
}

void shambhala_shutdown(sb_App* app)
{
    if (app->ui) {
        ui_shutdown(app->ui);
        free(app->ui);
        app->ui = NULL;
    }

    if (app->renderer) {
        render_shutdown(app->renderer);
        free(app->renderer);
        app->renderer = NULL;
    }

    if (app->scene) {
        scene_destroy(app->scene);
        free(app->scene);
        app->scene = NULL;
    }

    app->running = 0;
}

// WHERE IT ALL HAPPENS!!!
void shambhala_update(sb_App* app)
{
    // Begin UI frame
    ui_begin(app->ui);

    // Draw UI windows
    ui_draw_demo_window(app->ui);
    ui_draw_scene_window(app->ui, app->scene->object_count);

    // End UI frame
    ui_end(app->ui);

    // Clear screen first, update scene, and then draw the scene.
    render_begin_frame(app->renderer, 0.2f, 0.3f, 0.4f, 1.0f);
    scene_update(app->scene);
    render_draw(app->renderer, app->scene);
}

void shambhala_render(sb_App* app)
{
    ui_render(app->ui, app->renderer);
    render_present(app->renderer);
}

void shambhala_run(sb_App* app)
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
