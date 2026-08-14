#include "mesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t align4(uint64_t size)
{
    return (size + 3) & ~(uint64_t)3;
}

sb_Mesh mesh_create(WGPUDevice device, WGPUQueue queue, mfloat_t* vertices, uint16_t vertex_count, const uint16_t* indices, uint16_t index_count)
{
    sb_Mesh mesh = { 0 };
    mesh.vertex_count = vertex_count;
    mesh.index_count = index_count;

    // 3 floats/vertex is always a multiple of 4 bytes    uint64_t vertex_bytes = (uint64_t)vertex_count * 3 * sizeof(mfloat_t);

    WGPUBufferDescriptor vertex_desc = { 0 };
    vertex_desc.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
    vertex_desc.size = vertex_bytes;

    mesh.vertex_buffer = wgpuDeviceCreateBuffer(device, &vertex_desc);
    wgpuQueueWriteBuffer(queue, mesh.vertex_buffer, 0, vertices, vertex_bytes);

    if (indices != NULL && index_count > 0) {
        uint64_t index_bytes = (uint64_t)index_count * sizeof(uint16_t);
        uint64_t padded_bytes = align4(index_bytes);

        WGPUBufferDescriptor index_desc = { 0 };
        index_desc.usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst;
        index_desc.size = padded_bytes;

        mesh.index_buffer = wgpuDeviceCreateBuffer(device, &index_desc);

        if (padded_bytes == index_bytes) {
            wgpuQueueWriteBuffer(queue, mesh.index_buffer, 0, indices, index_bytes);
        } else {
            // wgpuQueueWriteBuffer requires a write size that's a multiple of 4
            uint8_t* padded = (uint8_t*)calloc(1, padded_bytes);
            memcpy(padded, indices, index_bytes);
            wgpuQueueWriteBuffer(queue, mesh.index_buffer, 0, padded, padded_bytes);
            free(padded);
        }
    }

    return mesh;
}

void mesh_destroy(sb_Mesh* mesh)
{
    if (mesh->index_buffer) {
        wgpuBufferRelease(mesh->index_buffer);
        mesh->index_buffer = NULL;
    }

    if (mesh->vertex_buffer) {
        wgpuBufferRelease(mesh->vertex_buffer);
        mesh->vertex_buffer = NULL;
    }

    mesh->vertex_count = 0;
    mesh->index_count = 0;
}

void mesh_draw(const sb_Mesh* mesh, WGPURenderPassEncoder pass)
{
    uint64_t vertex_bytes = (uint64_t)mesh->vertex_count * 3 * sizeof(mfloat_t);
    wgpuRenderPassEncoderSetVertexBuffer(pass, 0, mesh->vertex_buffer, 0, vertex_bytes);

    if (mesh->index_count > 0) {
        uint64_t index_bytes = (uint64_t)mesh->index_count * sizeof(uint16_t);
        wgpuRenderPassEncoderSetIndexBuffer(pass, mesh->index_buffer, WGPUIndexFormat_Uint16, 0, index_bytes);
        wgpuRenderPassEncoderDrawIndexed(pass, mesh->index_count, 1, 0, 0, 0);
    } else {
        wgpuRenderPassEncoderDraw(pass, mesh->vertex_count, 1, 0, 0);
    }
}
