#pragma once
#include "../Maths/Maths.h"

namespace LacertaEngine
{
    
struct ShadowMapLightConstantBuffer
{
    Matrix4x4 ViewMatrix;
    Matrix4x4 ProjectionMatrix;
};
    
}