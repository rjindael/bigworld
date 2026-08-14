#ifndef SHADER_H
#define SHADER_H

#include <webgpu/webgpu.h>

// A shader owns a full WebGPU render pipeline
typedef struct {
    WGPUDevice device; // kept so shader_destroy() doesn't need it passed back in
    WGPUQueue queue;   // kept so shader_set_*() doesn't need it passed back in

    WGPUShaderModule vertex_module;
    WGPUShaderModule fragment_module;
    WGPUBindGroupLayout bind_group_layout;
    WGPUPipelineLayout pipeline_layout;
    WGPURenderPipeline pipeline;
    WGPUBindGroup bind_group;
    WGPUBuffer uniform_buffer;
} sb_Shader;

////////////////// LIFECYCLE //////////////////

sb_Shader shader_create(WGPUDevice device, WGPUQueue queue, WGPUTextureFormat surface_format, const char* vertex_src, const char* fragment_src);
void shader_destroy(sb_Shader* shader);

////////////////// OPERATIONS //////////////////

void shader_use(const sb_Shader* shader, WGPURenderPassEncoder pass);

// Uniform setters
// name unused cuz wgpu doesn't have name-based uniform lookup like glUniform*, so we just write to the fixed offsets defined above
void shader_set_float(const sb_Shader* shader, const char* name, float value);
void shader_set_vec3(const sb_Shader* shader, const char* name, float x, float y, float z);

#endif // SHADER_H
