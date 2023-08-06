#include "MathUtilities.h"

float LacertaEngine::MathUtilities::Remap(float value, float inMin, float inMax, float outMin, float outMax)
{
    return outMin + (value - inMin) * (outMax - outMin) / (inMax - inMin);
}
