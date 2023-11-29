#include "random.hpp"
#include <random>

thread_local std::mt19937 Random::randomEngine;
std::uniform_real_distribution<float> Random::distribution;
