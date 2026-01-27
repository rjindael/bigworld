#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

// glCreateShader -> glShaderSource -> glCompileShader
GLuint shader_compile(const char* source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

#ifdef STRICT_CHECKS
    // check on the compile status for debugging
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "%s shader compilation error:\n%s\n", (type == GL_VERTEX_SHADER) ? "vert" : "frag", info_log);
        return 0;
    }
#endif

    return shader;
}

// glAttachShader + glLinkProgram
GLuint shader_link_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

#ifdef STRICT_CHECKS
    // check if it actually linked
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        fprintf(stderr, "Shader program linking error:\n%s\n", info_log);
        return 0;
    }
#endif

    return program;
}

// Compiles & links a shader
sb_Shader shader_create(const char* vertex_src, const char* fragment_src)
{
    sb_Shader shader = { 0 };

    GLuint vertex_shader = shader_compile(vertex_src, GL_VERTEX_SHADER);
    if (vertex_shader == 0) {
        return shader;
    }

    GLuint fragment_shader = shader_compile(fragment_src, GL_FRAGMENT_SHADER);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        return shader;
    }

    shader.id = shader_link_program(vertex_shader, fragment_shader);

    // delet shaders as they r now linked into the program
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader;
}

// glDeleteProgram
void shader_destroy(sb_Shader* shader)
{
    if (shader->id != 0) {
        glDeleteProgram(shader->id);
        shader->id = 0;
    }
}

// glUseProgram
void shader_use(const sb_Shader* shader)
{
    glUseProgram(shader->id);
}

// Returns the ID of the program for the given  shader
GLuint shader_get_program(const sb_Shader* shader)
{
    return shader->id;
}

void shader_set_float(const sb_Shader* shader, const char* name, float value)
{
    GLint location = glGetUniformLocation(shader->id, name);
    glUniform1f(location, value);
}

void shader_set_vec3(const sb_Shader* shader, const char* name, float x, float y, float z)
{
    GLint location = glGetUniformLocation(shader->id, name);
    glUniform3f(location, x, y, z);
}
