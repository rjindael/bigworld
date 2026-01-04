#include "mesh.h"
#include <stdio.h>

Mesh mesh_create(const float* vertices, int vertex_count, const unsigned int* indices, int index_count)
{
    Mesh mesh = { 0 };
    mesh.vertex_count = vertex_count;
    mesh.index_count = index_count;

    // generate&bind VAO
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // generate&bind VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(float), vertices, GL_STATIC_DRAW);

    // setup vertex attributes (our positions are vec3s)
    // REF: https://learnopengl.com/Getting-started/Hello-Triangle#:~:text=The%20function%20glVertexAttribPointer%20has%20quite%20a%20few%20parameters%20so%20let%27s%20carefully%20walk%20through%20them%3A
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // generate&bind EBO if indices are provided
    if (indices != NULL && index_count > 0) {
        glGenBuffers(1, &mesh.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    }

    // unbind (by binding to null aka 0)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return mesh;
}

void mesh_destroy(Mesh* mesh)
{
    // element buffer
    if (mesh->ebo != 0) {
        glDeleteBuffers(1, &mesh->ebo);
        mesh->ebo = 0;
    }

    // vertex buffer
    if (mesh->vbo != 0) {
        glDeleteBuffers(1, &mesh->vbo);
        mesh->vbo = 0;
    }

    // vertex array
    if (mesh->vao != 0) {
        glDeleteVertexArrays(1, &mesh->vao);
        mesh->vao = 0;
    }

    // reset counts
    mesh->vertex_count = 0;
    mesh->index_count = 0;
}

// glBindVertexArray (to the mesh's VAO)
void mesh_bind(const Mesh* mesh)
{
    glBindVertexArray(mesh->vao);
}

// glBindVertexArray to nothing
void mesh_unbind(void)
{
    glBindVertexArray(0);
}

// bind it, draw it, unbind it
void mesh_draw(const Mesh* mesh)
{
    mesh_bind(mesh);

    if (mesh->index_count > 0) {
        glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count / 3);
    }

    mesh_unbind();
}
