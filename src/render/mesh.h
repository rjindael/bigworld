#ifndef MESH_H
#define MESH_H

#include <glad/gl.h>
#include <mathc.h>
#include <stdint.h>

// Simple mesh that can hold vertex data

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    uint16_t vertex_count;
    uint16_t index_count;
} sb_Mesh;

////////////////// LIFECYCLE //////////////////

sb_Mesh mesh_create(mfloat_t* vertices, uint16_t vertex_count, const uint16_t* indices, uint16_t index_count);
void mesh_destroy(sb_Mesh* mesh);

////////////////// OPERATIONS //////////////////

void mesh_bind(const sb_Mesh* mesh);
void mesh_unbind(void);
void mesh_draw(const sb_Mesh* mesh);

#endif // MESH_H
