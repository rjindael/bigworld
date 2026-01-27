// SPDX-License-Identifier: GPL-2.0-or-later
#include "input.h"
#include "shambhala.h"
#include "render/engine.h"
#include "world/scene.h"
#include "ui.h"

#include <stdio.h>
#include <SDL3/SDL.h>

// For now home, to all SDL event handling

void input_process(sb_App* app)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            printf("Received quit event, shutting down...\n");
            app->running = 0;
            break;

        case SDL_EVENT_MOUSE_MOTION:
            ui_input_mousemove(app->ui, event.motion.x, event.motion.y);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            ui_input_mousedown(app->ui,
                               event.button.x,
                               event.button.y,
                               event.button.button == SDL_BUTTON_LEFT  ? MU_MOUSE_LEFT :
                               event.button.button == SDL_BUTTON_RIGHT ? MU_MOUSE_RIGHT :
                                                                         MU_MOUSE_MIDDLE);
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            ui_input_mouseup(app->ui,
                             event.button.x,
                             event.button.y,
                             event.button.button == SDL_BUTTON_LEFT  ? MU_MOUSE_LEFT :
                             event.button.button == SDL_BUTTON_RIGHT ? MU_MOUSE_RIGHT :
                                                                       MU_MOUSE_MIDDLE);
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            ui_input_scroll(app->ui, event.wheel.x * 30, event.wheel.y * -30);
            break;

        case SDL_EVENT_KEY_DOWN:
            printf("Key down: %s\n", SDL_GetKeyName(event.key.key));
            if (event.key.key == SDLK_ESCAPE) {
                app->running = 0;
            } else if (event.key.key == SDLK_D) {
                printf("Drawing debug triangle\n");
                if (!_DBG_scene_make_tri(app->scene)) {
                    fprintf(stderr, "Failed to create debug triangle\n");
                }
            } else if (event.key.key == SDLK_S) {
                printf("Making debug triangle SPIN!!!\n");
                if (!_DBG_scene_make_tri_spin(app->scene)) {
                    fprintf(stderr, "Failed to make debug triangle spin (make sure to draw only one......)\n");
                }
            }
            break;

        case SDL_EVENT_WINDOW_RESIZED:
            render_resize(app->renderer, &event.window);
            break;
        }
    }
}