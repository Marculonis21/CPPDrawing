#pragma once

#include <random>

class Random
{
    thread_local static std::mt19937 randomEngine;
    static std::uniform_real_distribution<float> distribution;

public:

    /**
    *  @brief  Returns random number between 0.0 and 1.0
    */
    inline static float random()
    {
        return distribution(randomEngine);
    }

    /* inline static double randomDouble(float min, float max) */
    /* { */
    /*     return min + (max-min) * randomDouble(); */
    /* } */

    /* inline static int randomInt(int min, int max) */
    /* { */
    /*     return min + ((int)(max*doubleDistribution(randomEngine)) % (max - min + 1)); */
    /* } */

    /* inline static glm::vec3 randomVector(float min, float max) */
    /* { */
    /*     return glm::vec3{randomDouble(min, max), */ 
    /*                      randomDouble(min, max), */
    /*                      randomDouble(min, max)}; */
    /* } */

    /* inline static glm::vec3 randomOnUnitSphere() */
    /* { */
    /*     /1* return glm::sphericalRand(1.0); *1/ */

    /*     auto r1 = randomDouble(); */
    /*     auto r2 = randomDouble(); */
    /*     auto x = cos(2*M_PI*r1)*2*sqrt(r2*(1-r2)); */
    /*     auto y = sin(2*M_PI*r1)*2*sqrt(r2*(1-r2)); */
    /*     auto z = 1 - 2*r2; */

    /*     return glm::vec3{x,y,z}; */
    /* } */
};
