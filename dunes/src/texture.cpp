#include "texture.hpp"

Texture2D::Texture2D(GLsizei width, GLsizei height, GLuint slot, GLenum format)
{
    this->width = width;
    this->height = height;
    this->format = format;
    this->bindSlot = slot;

    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureStorage2D(textureID, 1, format, width, height);
    glBindImageTexture(slot, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

Texture2D::~Texture2D() noexcept
{
    glDeleteTextures(1, &textureID);
}

void Texture2D::AddData(GLenum pixelFormat, GLenum type, void* pixels)
{
    glTextureSubImage2D(textureID, 0, 0, 0, width, height, pixelFormat, type, pixels);
}

void Texture2D::Activate()
{
    glActiveTexture(GL_TEXTURE0+bindSlot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}
