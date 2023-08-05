#pragma once
#include "Component.h"
#include "../Core.h"
#include "Transform.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API GameObject
{
public:
    GameObject();
    ~GameObject();

    void SetPosition(Vector3 position);
    void SetScale(Vector3 scale);
    // TODO SetRotation

    Transform* GetTransform() { return m_transform; }

    void AddComponent(Component* comp);
    
    std::list<Component*> GetComponentsList() { return m_components; }
    
private:
    Transform* m_transform;
    std::list<Component*> m_components;
};
    
}
