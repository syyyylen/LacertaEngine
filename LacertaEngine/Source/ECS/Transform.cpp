#include "Transform.h"

namespace LacertaEngine
{
    
Transform::Transform()
{
    m_transformMatrix.SetIdentity();
}

Transform::~Transform()
{
}

void Transform::SetPosition(Vector3 position)
{
    m_transformMatrix.SetTranslation(position);
}

// void Transform::SetRotation(Vector3 rotation)
// {
//    m_transformMatrix.SetRotation(rotation);
// }

void Transform::SetScale(Vector3 scale)
{
    m_transformMatrix.SetScale(scale);
}

Vector3 Transform::TransformPosition(Vector3 position)
{
    Vector4 worldPos(position.X, position.Y, position.Z, 1.0f);
    Vector4 transformedPos = m_transformMatrix * worldPos;
    return Vector3(transformedPos.X, transformedPos.Y, transformedPos.Z);
}

Vector3 Transform::InverseTransformPosition(Vector3 position)
{
    Matrix4x4 inverseMatrix = m_transformMatrix;
    inverseMatrix.Inverse();
    Vector4 localPos(position.X, position.Y, position.Z, 1.0f);
    Vector4 transformedPos = inverseMatrix * localPos;
    return Vector3(transformedPos.X, transformedPos.Y, transformedPos.Z);
}

Vector3 Transform::GetForwardVector()
{
    return m_transformMatrix.GetZDirection();
}
    
}
