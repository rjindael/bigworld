#ifndef OBJECT_H
#define OBJECT_H

// All derived objs will inherit from this base obj
// For now, every object simply represents a mesh in the 3D scene and have a VS/FS attached in form of a Shader
// In the future, they will have more properties and attributes

#include "render/gfx/vec3.h"
#include "render/mesh.h"
#include "render/shader.h"

#include <stdbool.h>

// fwd declaration - Scene is defined in scene.h
struct Scene;

typedef struct {
    char id[32];
    Mesh mesh;
    Shader shader;
    Vec3 position;
    Vec3 rotation; // in degrees
    Vec3 scale;
    struct Scene* scene; // pointer to parent scene, assigned once added to a scene
    bool _DBG_spinning;
} Object;

////////////////// LIFECYCLE //////////////////

Object object_create(const Mesh* mesh, const Shader* shader, const Vec3* position, const Vec3* rotation, const Vec3* scale);
void object_destroy(Object* object);
void object_update(Object* object);

////////////////// OPERATIONS //////////////////

void object_set_position(Object* object, const Vec3* position);
void object_set_rotation(Object* object, const Vec3* rotation);
void object_set_scale(Object* object, const Vec3* scale);

#endif // OBJECT_H