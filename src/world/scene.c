#include "scene.h"
#include "object.h"
#include "io.h"

#include <string.h>
#include <stdio.h>

Scene scene_create()
{
    Scene scene = { 0 };
    scene.objects = NULL;
    scene.object_count = 0;
    return scene;
}

void scene_destroy(Scene* scene)
{
    for (size_t i = 0; i < scene->object_count; i++) {
        object_destroy(&scene->objects[i]);
    }

    free(scene->objects);

    scene->objects = NULL;
    scene->object_count = 0;
}

void scene_add_object(Scene* scene, const Object* object)
{
    scene->objects = (Object*)realloc(scene->objects, sizeof(Object) * (scene->object_count + 1));
    scene->objects[scene->object_count] = *object;
    scene->objects[scene->object_count].scene = scene;
    scene->object_count++;
}

void scene_remove_object(Scene* scene, const char* id)
{
    // lookup index by id
    // TODO: this could be implemented more efficiently with a hash map, but we don't expect many objects for now
    for (size_t i = 0; i < scene->object_count; i++) {
        if (strncmp(scene->objects[i].id, id, 32) == 0) {
            _scene_remove_object(scene, i);
            return;
        }
    }
}

void _scene_remove_object(Scene* scene, size_t index)
{
    if (index >= scene->object_count) {
        return; // invalid index
    }

    for (size_t i = index; i < scene->object_count - 1; i++) {
        scene->objects[i] = scene->objects[i + 1];
    }

    scene->object_count--;
    scene->objects = (Object*)realloc(scene->objects, sizeof(Object) * scene->object_count);
}

void scene_update(Scene* scene)
{
    // process all objects in the scene
    for (size_t i = 0; i < scene->object_count; i++) {
        object_update(&scene->objects[i]);
    }
}

bool _DBG_scene_make_tri(Scene* scene)
{
    // first need to make it's shader
    char* vertex_src = io_read_file("shaders/vert/tri.glsl");
    char* fragment_src = io_read_file("shaders/frag/tri.glsl");

    if (!vertex_src || !fragment_src) {
        fprintf(stderr, "Failed to load shader files\n");
        free(vertex_src);
        free(fragment_src);
        return false;
    }

    Shader program = shader_create(vertex_src, fragment_src);

    free(vertex_src);
    free(fragment_src);

    if (program.id == 0) {
        fprintf(stderr, "Failed to create shader program\n");
        return false;
    }

    // then create the mesh
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    Vec3 vertices[] = {
        { -0.5f, 0.5f, 0.0f }, // left
        { 0.5f, 0.5f, 0.0f }, // right
        { 0.0f, -0.5f, 0.0f } // bottom
    };

    Mesh mesh = mesh_create(vertices, 3, NULL, 0);

    // and now, create the object
    Vec3 position = { 0.0f, 0.0f, 0.0f };
    Vec3 rotation = { 0.0f, 0.0f, 0.0f };
    Vec3 scale = { 1.0f, 1.0f, 1.0f };
    Object obj = object_create(&mesh, &program, &position, &rotation, &scale);

    // add it to the scene
    scene_add_object(scene, &obj);

    return true;
}

bool _DBG_scene_make_tri_spin(Scene* scene)
{
    if (scene->object_count != 1) {
        return false;
    }

    scene->objects[0]._DBG_spinning = !scene->objects[0]._DBG_spinning;

    return true;
}