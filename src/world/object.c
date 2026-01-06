#include "object.h"
#include "scene.h"

#include <stdio.h>
#include <stdlib.h>

Object object_create(const Mesh* mesh, const Shader* shader, const Vec3* position, const Vec3* rotation, const Vec3* scale)
{
    Object object = { 0 };
    object.mesh = *mesh;
    object.shader = *shader;
    object.position = *position;
    object.rotation = *rotation;
    object.scale = *scale;

    // generate an unique ID for the object
    for (int i = 0; i < 32; ++i) {
        object.id[i] = "0123456789abcdef"[rand() % 16];
    }

    printf("Created new obj @ pos(%.3f, %.3f, %.3f)/rot(%.3f, %.3f, %.3f), ID: '%s'\n",
           position->x,
           position->y,
           position->z,
           rotation->x,
           rotation->y,
           rotation->z,
           object.id);

    return object;
}

void object_destroy(Object* object)
{
    scene_remove_object(object->scene, object->id);
    mesh_destroy(&object->mesh);
    shader_destroy(&object->shader);
}

void object_set_position(Object* object, const Vec3* position)
{
    object->position = *position;
}

void object_set_rotation(Object* object, const Vec3* rotation)
{
    object->rotation = *rotation;
}

void object_set_scale(Object* object, const Vec3* scale)
{
    object->scale = *scale;
}

void object_update(Object* object)
{
    if (object->_DBG_spinning) {
        object->rotation.y += 1.0f;
        if (object->rotation.y >= 360.0f) {
            object->rotation.y -= 360.0f;
        }

        // Set rotation uniform
        shader_set_float(&object->shader, "uRotationY", object->rotation.y);
    }
}