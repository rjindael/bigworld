// SPDX-License-Identifier: GPL-2.0-or-later
#include "shambhala.h"

#include <stdio.h>
#include <SDL3/SDL.h>

int main(int argc, char* argv[])
{
    sb_App app;

    // the backbone of shambhala is SDL
    // enable video, audio, gamepad, haptic, and sensor support (including events and joystick automagically.)
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC | SDL_INIT_SENSOR)) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 0;
    }

    if (!shambhala_init(&app, 800, 600, "shambhala")) {
        fprintf(stderr, "Failed to initialize shambhala\n");
        return 1;
    }

    printf("Started Shambhala v%s!\n", VERSION);

    shambhala_run(&app);
    shambhala_shutdown(&app);

    printf("Goodbye!\n");

    return 0;
}
