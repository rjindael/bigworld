#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>

typedef struct {
    GLuint id;
} Shader;

////////////////// LIFECYCLE //////////////////

Shader shader_create(const char* vertex_src, const char* fragment_src);
void shader_destroy(Shader* shader);

////////////////// OPERATIONS //////////////////

void shader_use(const Shader* shader);
GLuint shader_get_program(const Shader* shader);
GLuint shader_compile(const char* source, GLenum type); // should probably be abstracted away too
GLuint shader_link_program(GLuint vertex_shader, GLuint fragment_shader);

// Uniform setters
void shader_set_float(const Shader* shader, const char* name, float value);
void shader_set_vec3(const Shader* shader, const char* name, float x, float y, float z);

#endif // SHADER_H
