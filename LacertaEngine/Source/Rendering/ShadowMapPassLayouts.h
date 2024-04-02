#pragma once
#include "../Maths/Maths.h"

namespace LacertaEngine
{
    
struct ShadowMapLightConstantBuffer
{
    Matrix4x4 ViewMatrix[4];
    Matrix4x4 ProjectionMatrix[4];
};
    
}