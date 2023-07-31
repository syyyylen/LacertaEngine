#include "Random.h"
#include <random>

namespace LacertaEngine
{
float Random::RandomFloatRange(float minRange, float maxRange)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distribution(minRange, maxRange);
    
    return distribution(gen);
}
}
