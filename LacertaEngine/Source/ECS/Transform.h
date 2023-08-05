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

    void SetPosition(Vector3 position);
    void SetScale(Vector3 scale);
    // TODO SetRotation

    Vector3 TransformPosition(Vector3 position);
    Vector3 InverseTransformPosition(Vector3 position);
    Vector3 GetForwardVector();

    void SetTransformMatrix(Matrix4x4 mat) { m_transformMatrix = mat; }
    Matrix4x4 GetTransformMatrix() { return m_transformMatrix; }

private:
    Matrix4x4 m_transformMatrix;
};
    
}
