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

    // tell the GPU the format of each vertex
    // each vertex is 3 floats (x, y, z) packed tightly together
    // location 0 matches "layout (location = 0) in vec3 aPos" in the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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