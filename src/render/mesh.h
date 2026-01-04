#ifndef MESH_H
#define MESH_H

#include <glad/gl.h>

// simple mesh that can hold vertex data
typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int vertex_count;
    int index_count;
} Mesh;

// LIFECYCLE //
Mesh mesh_create(const float* vertices, int vertex_count, const unsigned int* indices, int index_count);
void mesh_destroy(Mesh* mesh);

// OPERATIONS //
void mesh_bind(const Mesh* mesh);
void mesh_unbind(void);
void mesh_draw(const Mesh* mesh);

#endif // MESH_H
