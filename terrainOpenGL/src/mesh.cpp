#include "mesh.hpp"
#include <iostream>

Mesh::Mesh(size_t count, float quadSize, bool usePatches)
{
    this->count = count;
    this->quadSize = quadSize;
    this->usePatches = usePatches;

    genQuadPlane();
    init();
    std::cout << "mesh generated" << std::endl;
}
Mesh::~Mesh()
{
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &indexBuffer);
	glDeleteVertexArrays(1, &arrayObj);
}

void Mesh::genQuadPlane()
{
    const float max = quadSize*count;

    for (size_t y = 0; y < count+1; y++) {
        for (size_t x = 0; x < count+1; ++x) {
            g_vertex_buffer_data.push_back(quadSize*x);
            g_vertex_buffer_data.push_back(0);
            g_vertex_buffer_data.push_back(quadSize*y);

            g_uv_buffer_data.push_back((quadSize*x)/max);
            g_uv_buffer_data.push_back((quadSize*y)/max);
        }
    }

    if (usePatches) {
        // PATCHES
        for (int y = 0; y < count; y++) {
            for (int x = 0; x < count; ++x) {
                g_index_buffer_data.push_back((y+1)*(count+1)+ x);
                g_index_buffer_data.push_back((y+1)*(count+1)+ x+1);
                g_index_buffer_data.push_back(y*(count+1)    + x+1);
                g_index_buffer_data.push_back(y*(count+1)    + x);
            }
        }
    }
    else {

        for (int y = 0; y < count; y++) {
            for (int x = 0; x < count; ++x) {
                g_index_buffer_data.push_back(y*(count+1)+(count+1)+x);
                g_index_buffer_data.push_back(y*(count+1)+ x+1);
                g_index_buffer_data.push_back(y*(count+1)+ x);

                g_index_buffer_data.push_back(y*(count+1)+(count+1)+x);
                g_index_buffer_data.push_back(y*(count+1)+(count+1)+x+1);
                g_index_buffer_data.push_back(y*(count+1)+ x+1);
            }
        }
    }
}
void Mesh::init()
{
    glCreateVertexArrays(1, &arrayObj);
	glBindVertexArray(arrayObj);

    glCreateBuffers(1, &indexBuffer);
    glNamedBufferData(indexBuffer, sizeof(GL_UNSIGNED_INT)*g_index_buffer_data.size(), g_index_buffer_data.data(), GL_STATIC_DRAW);
    glVertexArrayElementBuffer(arrayObj, indexBuffer);
    this->indexCount = g_index_buffer_data.size();

    glCreateBuffers(1, &vertexBuffer);
    glNamedBufferData(vertexBuffer, sizeof(GL_FLOAT)*g_vertex_buffer_data.size(), g_vertex_buffer_data.data(), GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(arrayObj, 0, vertexBuffer, 0, 3*sizeof(GL_FLOAT));

    glCreateBuffers(1, &uvbuffer);
    glNamedBufferData(uvbuffer, sizeof(GL_FLOAT)*g_uv_buffer_data.size(), g_uv_buffer_data.data(), GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(arrayObj, 1, uvbuffer, 0, 2*sizeof(GL_FLOAT));

    glVertexArrayAttribFormat(arrayObj, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(arrayObj,0, 0);
    glEnableVertexArrayAttrib(arrayObj, 0);

    glVertexArrayAttribFormat(arrayObj, 1, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(arrayObj, 1, 1);
    glEnableVertexArrayAttrib(arrayObj, 1);
}

void Mesh::activate()
{
    glBindVertexArray(arrayObj);
}
