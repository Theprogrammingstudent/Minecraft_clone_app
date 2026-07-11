#include "Texture.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

Texture::Texture(const std::string& filepath)
{
    // tell stb_image to flip the image vertically on load
    // OpenGL expects UV origin at bottom-left
    // PNG files store pixels top-left first — so we flip
    stbi_set_flip_vertically_on_load(true);

    // load the image from disk into CPU memory
    // channels will be filled with how many colour channels the image has
    int channels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);

    // ADD THESE DEBUG LINES
    std::cout << "Trying to load: " << filepath << std::endl;
    std::cout << "Width: " << width << " Height: " << height << " Channels: " << channels << std::endl;

    if (!data)
    {
        // stbi_failure_reason() tells you exactly why it failed
        std::cerr << "stbi_load failed: " << stbi_failure_reason() << std::endl;
        return;
    }

    if (!data)
    {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return;
    }

    // generate one texture object on the GPU
    glGenTextures(1, &ID);

    // bind it so subsequent calls apply to this texture
    glBindTexture(GL_TEXTURE_2D, ID);

    // what to do when UV coords go outside 0.0-1.0
    // GL_REPEAT tiles the texture — fine for block faces
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // how to scale the texture when it's displayed at a different size
    // GL_NEAREST keeps pixels sharp — GL_LINEAR blurs them
    // GL_NEAREST is correct for a Minecraft-style pixelated look
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // determine the format based on how many channels the image has
    // RGB = no transparency, RGBA = has transparency
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    // upload the pixel data from CPU memory to GPU VRAM
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // generate mipmaps — smaller versions of the texture for distant blocks
    // prevents flickering when blocks are far away
    glGenerateMipmap(GL_TEXTURE_2D);

    // free the CPU copy — GPU has its own copy now
    stbi_image_free(data);
}

void Texture::bind(unsigned int slot) const
{
    // activate the texture slot then bind our texture to it
    // GL_TEXTURE0 + slot lets you use multiple textures at once if needed
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}