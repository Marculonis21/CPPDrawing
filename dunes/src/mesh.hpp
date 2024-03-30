#ifndef MESH_HPP

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Mesh
{
public:
    GLsizei indexCount;

    Mesh(size_t count, float quadSize, bool usePatches);
    virtual ~Mesh();
    void activate();
    void deactivate();

private:
    void genQuadPlane();
    void init();

    GLuint arrayObj, vertexBuffer, uvbuffer, indexBuffer;
    size_t count;
    float quadSize;

    bool usePatches;

    std::vector<GLfloat> g_vertex_buffer_data = {};
    std::vector<GLuint> g_index_buffer_data = {};
    std::vector<GLfloat> g_uv_buffer_data = {};
};

#define MESH_HPP 
#endif /* ifndef MESH_HPP  */
