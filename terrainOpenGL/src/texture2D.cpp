#include "texture2D.hpp"
#include <GL/gl.h>
#include <cassert>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include "stb/stb_image.h"

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

    std::vector<GLubyte> tData(this->width*this->height*4, 0);
    for (int i = 0; i < tData.size()/4; ++i) {
        tData[i*4+0] = (GLubyte)0;
        tData[i*4+1] = (GLubyte)0;
        tData[i*4+2] = (GLubyte)0;
        tData[i*4+3] = (GLubyte)0;
    }
    AddData(GL_RGBA, GL_UNSIGNED_BYTE, tData.data());
}

Texture2D::Texture2D(const std::string &path, GLuint slot, GLenum pixelFormat) {
    int _width, _height, _nrChannels;
    unsigned char *data = stbi_load(path.c_str(), &_width, &_height, &_nrChannels, 0);
    std::cout << "Loading texture ... " + path << std::endl;
    assert(data);

    this->width = _width;
    this->height = _height;
    this->format = GL_RGBA32F;
    this->bindSlot = slot;

    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureStorage2D(textureID, 1, format, width, height);
    glBindImageTexture(slot, textureID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

    AddData(pixelFormat, GL_UNSIGNED_BYTE, data);
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

void Texture2D::Data()
{
    GLubyte* pixels = new GLubyte[1024*1024*4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    int min = 256;
    int max = -1;

    for (int i = 0; i < 1024*1024; ++i) {
        if (pixels[i*4] > max) max = pixels[i*4];
        if (pixels[i*4] < min) min = pixels[i*4];
    }
    std::cout << "min: " << min << ", max: " << max << std::endl;
}
