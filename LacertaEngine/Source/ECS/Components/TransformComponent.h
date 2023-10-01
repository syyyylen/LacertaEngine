#pragma once
#include "../../Maths/Maths.h"
#include "../../Core.h"
#include "../Component.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API TransformComponent : public Component
{
public:
    TransformComponent();
    ~TransformComponent();

    void SetPosition(Vector3 position);
    void SetScale(Vector3 scale);
    // TODO SetRotation

    Vector3 TransformPosition(Vector3 position);
    Vector3 InverseTransformPosition(Vector3 position);
    Vector3 GetForwardVector();

     // TODO remove and use constructor
    void SetStartPosition(Vector3 position) { m_startPosition = position; }
    Vector3 GetStartPosition() { return m_startPosition; }

    Vector3 Position() { return m_transformMatrix.GetTranslation(); }
    Vector3 Scale() { return m_transformMatrix.GetScale(); }
    
    void SetTransformMatrix(Matrix4x4 mat) { m_transformMatrix = mat; }
    Matrix4x4 GetTransformMatrix() { return m_transformMatrix; }

private:
    Matrix4x4 m_transformMatrix;
    Vector3 m_startPosition;
};
    
}
