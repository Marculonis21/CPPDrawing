#ifndef NOISE_H
#define NOISE_H

#include "texture2D.hpp"
#include <glm/fwd.hpp>

class Noise
{
    float noise(float xPos, float yPos, int octave, float startAmplitude, float startFrequency);

public:
    void perlinToTexture(int x_quad_count, int y_quad_count, Texture2D &heightTexture, float frequency, int octaves);
};

#endif 
