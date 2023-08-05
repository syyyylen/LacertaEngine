#pragma once
#include "../Maths/Maths.h"
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Transform
{
public:
    Transform();
    ~Transform();

    void SetTransformMatrix(Matrix4x4 mat) { m_transformMatrix = mat; }
    Matrix4x4 GetTransformMatrix() { return m_transformMatrix; }

private:
    Matrix4x4 m_transformMatrix;
};
    
}
