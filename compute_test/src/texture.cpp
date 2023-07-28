#include "texture.hpp"
#include <iostream>

Texture2D::Texture2D(uint width, uint height, GLenum format)
{
    this->width = width;
    this->height = height;
    this->format = format;
    
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(textureID, 1, format, width, height);
}

void Texture2D::BindImageTexture(uint slot)
{
    glBindImageTexture(slot, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, format);
}

void Texture2D::ActiveTexture(uint slot)
{
    glActiveTexture(GL_TEXTURE0+slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture2D::FillData(GLenum pixel_format, GLenum type, const void* pixels)
{
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, pixel_format, type, pixels);
}
