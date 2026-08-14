#ifndef MESH_H
#define MESH_H

#include <webgpu/webgpu.h>
#include <mathc.h>
#include <stdint.h>

// Simple mesh that can hold vertex data. Vertices are always vec3 positions
// at attribute location 0 (see the pipeline built in shader.c).

typedef struct {
    WGPUBuffer vertex_buffer;
    WGPUBuffer index_buffer; // NULL if the mesh has no indices
    uint16_t vertex_count;
    uint16_t index_count;
} sb_Mesh;

////////////////// LIFECYCLE //////////////////

sb_Mesh mesh_create(WGPUDevice device, WGPUQueue queue, mfloat_t* vertices, uint16_t vertex_count, const uint16_t* indices, uint16_t index_count);
void mesh_destroy(sb_Mesh* mesh);

////////////////// OPERATIONS //////////////////

void mesh_draw(const sb_Mesh* mesh, WGPURenderPassEncoder pass);

#endif // MESH_H
