#ifndef SHAMBHALA_H
#define SHAMBHALA_H

#include "render/engine.h"
#include "world/scene.h"

#include <stdbool.h>

#define VERSION "1.0.0"

typedef struct {
    Renderer renderer;
    Scene scene;
    bool running;
} Shambhala;

////////////////// LIFECYCLE //////////////////

int shambhala_init(Shambhala* app, int width, int height, const char* title);
void shambhala_shutdown(Shambhala* app);
void shambhala_run(Shambhala* app);

////////////////// EVENTS //////////////////

void shambhala_update(Shambhala* app);
void shambhala_render(Shambhala* app);

#endif // SHAMBHALA_H
