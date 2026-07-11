#include "Renderer.h"
#include <glad/glad.h>

Renderer::Renderer()
{
    // create empty VAO, VBO and EBO on the GPU
    // no vertex data yet — uploadMesh() fills them
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the VAO so it records the buffer and attribute setup below
    glBindVertexArray(VAO);

    // bind buffers — empty for now, uploadMesh() will fill them
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // CHANGED — each vertex is now 5 floats (x, y, z, u, v) not 3
    // stride is the total size of one vertex in bytes
    // offset tells the GPU where each attribute starts within that vertex
    int stride = 5 * sizeof(float);

    // attribute 0 — position (x, y, z)
    // 3 floats, starts at byte 0 of each vertex
    // matches "layout (location = 0) in vec3 aPos" in vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // attribute 1 — texture coordinate (u, v)
    // 2 floats, starts at byte 12 (after the 3 position floats)
    // matches "layout (location = 1) in vec2 aTexCoord" in vertex shader
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // unbind VAO — good habit, prevents accidental modification
    glBindVertexArray(0);
}

void Renderer::uploadMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
    // store index count so draw() knows how many to render
    m_indexCount = (int)indices.size();

    // bind the VAO so our uploads are recorded
    glBindVertexArray(VAO);

    // upload vertex data to the VBO
    // GL_DYNAMIC_DRAW tells the GPU this data may change — good for chunk updates later
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),  // total bytes
        vertices.data(),                    // pointer to the data
        GL_DYNAMIC_DRAW);

    // upload index data to the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),  // total bytes
        indices.data(),                          // pointer to the data
        GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void Renderer::draw(int indexCount)
{
    glBindVertexArray(VAO);

    // draw using the index buffer
    // indexCount tells OpenGL how many indices to process
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}