#pragma once

class Renderer
{
public:
    unsigned int VAO, VBO;
    unsigned int EBO; // ADD THIS — index buffer, tells GPU which vertices to connect
    Renderer();
    void draw();
};