#ifndef SCENE_H
#define SCENE_H

#include <stdlib.h>

typedef struct sb_Object sb_Object;
typedef struct sb_Renderer sb_Renderer;

typedef struct sb_Scene {
    sb_Object* objects;
    size_t object_count;
    sb_Renderer* renderer; // used to create GPU resources for debug/dynamically-added objects
} sb_Scene;

////////////////// LIFECYCLE //////////////////

sb_Scene scene_create(sb_Renderer* renderer);
void scene_destroy(sb_Scene* scene);
void scene_update(sb_Scene* scene);

////////////////// OPERATIONS //////////////////

void scene_add_object(sb_Scene* scene, const sb_Object* object);
void scene_remove_object(sb_Scene* scene, const char* id);

// Internally used by scene_remove_object
// Call scene_remove_object instead, to reference by ID
void _scene_remove_object(sb_Scene* scene, size_t index);

// For debugging
// Internally, creates objects, adds to scene, and modifies them (which then gets processed each time the object then wil update)
// Called by input
int _DBG_scene_make_tri(sb_Scene* scene);
int _DBG_scene_make_tri_spin(sb_Scene* scene);

#endif // SCENE_H