#pragma once
#include <vector>
#include "ChunkMesher.h"

class Renderer
{
public:
    unsigned int VAO, VBO, EBO;

    Renderer();

    // uploads mesh data from the mesher to the GPU
    // called once after buildMesh() to push vertices and indices into VRAM
    void uploadMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // draws whatever mesh is currently uploaded
    // indexCount tells it how many indices to draw
    void draw(int indexCount);

private:
    // stored so draw() knows how many indices to render
    int m_indexCount = 0;
};