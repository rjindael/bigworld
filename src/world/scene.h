#ifndef SCENE_H
#define SCENE_H

#include "object.h"

#include <stdlib.h>

typedef struct Scene {
    Object* objects;
    size_t object_count;
} Scene;

////////////////// LIFECYCLE //////////////////

Scene scene_create();
void scene_destroy(Scene* scene);
void scene_update(Scene* scene);

////////////////// OPERATIONS //////////////////

void scene_add_object(Scene* scene, const Object* object);
void scene_remove_object(Scene* scene, const char* id);

// Internally used by scene_remove_object
// Call scene_remove_object instead, to reference by ID
void _scene_remove_object(Scene* scene, size_t index);

// For debugging
// Internally, creates objects, adds to scene, and modifies them (which then gets processed each time the object then wil update)
// Called by input
bool _DBG_scene_make_tri(Scene* scene);
bool _DBG_scene_make_tri_spin(Scene* scene);

#endif // SCENE_H