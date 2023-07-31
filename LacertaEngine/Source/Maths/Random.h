#pragma once
#include "../Core.h"

namespace LacertaEngine
{

class LACERTAENGINE_API Random
{
public:
    // TODO this is slow af, write better rdm functions when
    // TODO per frame calls will be needed 
    static float RandomFloatRange(float minRange, float maxRange);
};

}
