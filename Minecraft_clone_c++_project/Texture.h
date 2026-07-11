#pragma once
#include <string>

class Texture
{
public:
    // the OpenGL texture ID — used to bind the texture before drawing
    //unsigned int ID;
    unsigned int ID = 0;

    // width and height of the loaded image in pixels
    int width, height;

    // loads a PNG from disk and uploads it to the GPU
    // filepath is relative to where the .exe runs from
    Texture(const std::string& filepath);

    // binds this texture to a texture slot before a draw call
    // slot 0 is the default — you only need multiple slots for multi-texturing
    void bind(unsigned int slot = 0) const;

    // unbinds any texture — good habit after drawing
    void unbind() const;
};