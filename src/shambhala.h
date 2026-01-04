#ifndef SHAMBHALA_H
#define SHAMBHALA_H

#include "render/engine.h"
#include <stdbool.h>

typedef struct {
    Renderer renderer;
    bool running;
} Shambhala;

// LIFECYCLE //
int shambhala_init(Shambhala* app, int width, int height, const char* title);
void shambhala_shutdown(Shambhala* app);
void shambhala_run(Shambhala* app);

// EVENTS //
void shambhala_process_events(Shambhala* app);
void shambhala_update(Shambhala* app);
void shambhala_render(Shambhala* app);

#endif // SHAMBHALA_H
