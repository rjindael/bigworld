#include "input.h"

#include <stdio.h>

// For now home, to all SDL event handling

void input_process(Shambhala* app)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            printf("Received quit event, shutting down...\n");
            app->running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            printf("Key down: %s\n", SDL_GetKeyName(event.key.key));
            if (event.key.key == SDLK_ESCAPE) {
                app->running = false;
            } else if (event.key.key == SDLK_D) {
                printf("Drawing debug triangle\n");
                if (!_DBG_scene_make_tri(&app->scene)) {
                    fprintf(stderr, "Failed to create debug triangle\n");
                }
            } else if (event.key.key == SDLK_S) {
                printf("Making debug triangle SPIN!!!\n");
                if (!_DBG_scene_make_tri_spin(&app->scene)) {
                    fprintf(stderr, "Failed to make debug triangle spin (make sure to draw only one......)\n");
                }
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
