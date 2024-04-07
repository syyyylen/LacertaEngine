#pragma once
#include "../Maths/Maths.h"

namespace LacertaEngine
{
    
struct SkyBoxConstantBuffer
{
    Matrix4x4 ViewMatrix;
    Matrix4x4 ProjectionMatrix;
    float GlobalAmbient;
    float Padding[3];
};

struct PrefilterMapConstantBuffer
{
    Vector4 Roughness;
};

}