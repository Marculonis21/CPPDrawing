#ifndef _TEXTURE_HPP
#define _TEXTURE_HPP 

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

class Texture2D
{
    GLenum format;
    uint width, height;

public:
    GLuint textureID;
    Texture2D(uint width, uint height, GLenum format=GL_RGBA32F);
    void BindImageTexture(uint slot=0);
    void ActiveTexture(uint slot=0);

    void FillData(GLenum pixel_format, GLenum type, const void* pixels);
};

#endif 
