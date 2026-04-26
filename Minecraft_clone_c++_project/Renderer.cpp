#include "Renderer.h"
#include <glad/glad.h>

Renderer::Renderer()
{
    // a square is just two triangles sharing an edge
    // we define 4 corners instead of 6 vertices (which would duplicate two)
    float vertices[] = {
         0.5f,  0.5f, 0.0f,   // top right     — index 0
         0.5f, -0.5f, 0.0f,   // bottom right  — index 1
        -0.5f, -0.5f, 0.0f,   // bottom left   — index 2
        -0.5f,  0.5f, 0.0f    // top left      — index 3
    };

    // the EBO tells the GPU which vertices to connect into triangles
    // triangle 1: top right, bottom right, top left
    // triangle 2: bottom right, bottom left, top left
    // together they form a square — notice indices 1 and 3 are reused
    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // create the VAO, VBO and EBO on the GPU
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO); // ADD — generate the index buffer

    // bind VAO first — it records everything that follows
    glBindVertexArray(VAO);

    // upload vertex positions to the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ADD — upload indices to the EBO
    // GL_ELEMENT_ARRAY_BUFFER tells OpenGL this is an index buffer not vertex data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // tell the GPU the format of each vertex
    // location 0, 3 floats per vertex, no gaps between them
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::draw()
{
    glBindVertexArray(VAO);

    // CHANGED — glDrawElements instead of glDrawArrays
    // glDrawArrays just draws vertices in order
    // glDrawElements uses the index buffer to know which vertices to connect
    // 6 = total indices (two triangles x 3 corners each)
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}