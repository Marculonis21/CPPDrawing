#include "noise.hpp"

#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>

#include <algorithm>
#include <execution>
#include <iostream>

float Noise::noise(float xPos, float yPos, int octave, float startAmplitude, float startFrequency)
{
    // octave starts from 0
    float f = startFrequency/(octave+1);
    float amp = startAmplitude*glm::pow(0.5, octave);

    return amp*((glm::perlin(glm::vec2(xPos/f, yPos/f))+1)/2);
}

void Noise::perlinToTexture(int x_quad_count, int y_quad_count, Texture2D &heightTexture, 
                            float frequency, int octaves)
{
    float amplitude = 1;

    std::vector<GLubyte> heightData(y_quad_count*x_quad_count*4, 0);
    std::vector<GLubyte> colorData(y_quad_count*x_quad_count*4, 0);

    for (int i = 0; i < x_quad_count*y_quad_count; ++i) {

        float _x = (i % x_quad_count)*10;
        float _y = (i / x_quad_count)*10;

        float value = 0;

        float totalAmplitude = 0;
        for (float o = 0; o < octaves; ++o)  {
            value += noise(_x, _y, o, amplitude, frequency);

            totalAmplitude += amplitude*glm::pow(0.5, o);
        }

        value = glm::pow(value, 1.5);
        value = value/glm::pow(totalAmplitude, 1.5);

        heightData[i*4+0] = (GLubyte)(value*255);
        heightData[i*4+1] = (GLubyte)(value*255);
        heightData[i*4+2] = (GLubyte)(value*255);
        heightData[i*4+3] = (GLubyte)(value*255);
    }

    heightTexture.AddData(GL_RGBA, GL_UNSIGNED_BYTE, heightData.data());
}
