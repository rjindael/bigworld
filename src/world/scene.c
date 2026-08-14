#include "world/scene.h"
#include "world/object.h"
#include "render/engine.h"
#include "io.h"

#include <string.h>
#include <stdio.h>

sb_Scene scene_create(sb_Renderer* renderer)
{
    sb_Scene scene = { 0 };
    scene.objects = NULL;
    scene.object_count = 0;
    scene.renderer = renderer;
    return scene;
}

void scene_destroy(sb_Scene* scene)
{
    for (size_t i = 0; i < scene->object_count; i++) {
        object_destroy(&scene->objects[i]);
    }

    free(scene->objects);

    scene->objects = NULL;
    scene->object_count = 0;
}

void scene_add_object(sb_Scene* scene, const sb_Object* object)
{
    scene->objects = (sb_Object*)realloc(scene->objects, sizeof(sb_Object) * (scene->object_count + 1));
    scene->objects[scene->object_count] = *object;
    scene->objects[scene->object_count].scene = scene;
    scene->object_count++;
}

void scene_remove_object(sb_Scene* scene, const char* id)
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

void _scene_remove_object(sb_Scene* scene, size_t index)
{
    if (index >= scene->object_count) {
        return; // invalid index
    }

    for (size_t i = index; i < scene->object_count - 1; i++) {
        scene->objects[i] = scene->objects[i + 1];
    }

    scene->object_count--;
    scene->objects = (sb_Object*)realloc(scene->objects, sizeof(sb_Object) * scene->object_count);
}

void scene_update(sb_Scene* scene)
{
    // process all objects in the scene
    for (size_t i = 0; i < scene->object_count; i++) {
        object_update(&scene->objects[i]);
    }
}

int _DBG_scene_make_tri(sb_Scene* scene)
{
    // first need to make it's shader
    char* vertex_src = io_read_file("shaders/vert/tri.wgsl");
    char* fragment_src = io_read_file("shaders/frag/tri.wgsl");

    if (!vertex_src || !fragment_src) {
        fprintf(stderr, "Failed to load shader files\n");
        free(vertex_src);
        free(fragment_src);
        return 0;
    }

    sb_Renderer* renderer = scene->renderer;
    sb_Shader program = shader_create(renderer->device, renderer->queue, renderer->surface_format, vertex_src, fragment_src);

    free(vertex_src);
    free(fragment_src);

    if (program.pipeline == NULL) {
        fprintf(stderr, "Failed to create shader program\n");
        return 0;
    }

    // then create the mesh
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    mfloat_t vertices[] = {
        -0.5f, 0.5f, 0.0f,  // left
        0.5f, 0.5f, 0.0f,   // right
        0.0f, -0.5f, 0.0f   // bottom
    };

    sb_Mesh mesh = mesh_create(renderer->device, renderer->queue, vertices, 3, NULL, 0);

    // and now, create the object
    struct vec3 position = { 0.0f, 0.0f, 0.0f };
    struct vec3 rotation = { 0.0f, 0.0f, 0.0f };
    struct vec3 scale = { 1.0f, 1.0f, 1.0f };
    sb_Object obj = object_create(&mesh, &program, &position, &rotation, &scale);

    // add it to the scene
    scene_add_object(scene, &obj);

    return 1;
}

int _DBG_scene_make_tri_spin(sb_Scene* scene)
{
    if (scene->object_count != 1) {
        return 0;
    }

    scene->objects[0]._DBG_spinning = !scene->objects[0]._DBG_spinning;

    return 1;
}