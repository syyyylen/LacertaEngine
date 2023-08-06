#pragma once
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API MathUtilities
{
public:
    static float Remap(float value, float inMin, float inMax, float outMin, float outMax);
};
    
}
