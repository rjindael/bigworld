#ifndef SHAMBHALA_H
#define SHAMBHALA_H

#define VERSION "1.0.0"

typedef struct sb_Renderer sb_Renderer;
typedef struct sb_Scene sb_Scene;
typedef struct sb_UI sb_UI;

typedef struct sb_App {
    sb_Renderer* renderer;
    sb_Scene* scene;
    sb_UI* ui;
    int running;
} sb_App;

////////////////// LIFECYCLE //////////////////

int shambhala_init(sb_App* app, int width, int height, const char* title);
void shambhala_shutdown(sb_App* app);
void shambhala_run(sb_App* app);

////////////////// EVENTS //////////////////

void shambhala_update(sb_App* app);
void shambhala_render(sb_App* app);

#endif // SHAMBHALA_H
