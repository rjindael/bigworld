#ifndef OBJECT_H
#define OBJECT_H

#include "render/mesh.h"
#include "render/shader.h"
#include "render/gfx/color.h"
#include <mathc.h>

typedef struct sb_Scene sb_Scene;

typedef struct sb_Object {
    char id[32];
    sb_Scene* scene; // pointer to parent scene, assigned once added to a scene

    sb_Mesh mesh;
    sb_Shader shader;

    struct vec3 position;
    struct vec3 rotation; // in degrees
    struct vec3 scale;

    int _DBG_spinning;
} sb_Object;

////////////////// LIFECYCLE //////////////////

sb_Object object_create(const sb_Mesh* mesh, const sb_Shader* shader, const struct vec3* position, const struct vec3* rotation, const struct vec3* scale);
void object_destroy(sb_Object* object);
void object_update(sb_Object* object);

////////////////// OPERATIONS //////////////////

void object_set_position(sb_Object* object, const struct vec3* position);
void object_set_rotation(sb_Object* object, const struct vec3* rotation);
void object_set_scale(sb_Object* object, const struct vec3* scale);

#endif // OBJECT_H