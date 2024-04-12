#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP

#include <GL/glew.h>

class Texture2D
{
    GLuint textureID;
    GLsizei width;
    GLsizei height;

    GLenum format;
    GLuint bindSlot;

public:
    Texture2D(GLsizei width, GLsizei height, GLuint slot=0, GLenum format=GL_RGBA32F);
    ~Texture2D() noexcept;
    void AddData(GLenum pixelFormat, GLenum type, void* pixels);
    void Activate();
    void Data();
};

#endif 
