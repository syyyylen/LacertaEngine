#include "GameObject.h"

namespace LacertaEngine
{
    
GameObject::GameObject()
{
    m_transform = new Transform();
    m_transform->GetTransformMatrix().SetIdentity();
}

GameObject::~GameObject()
{
}

void GameObject::SetPosition(Vector3 position)
{
    if(m_transform)
        m_transform->SetPosition(position);
}

void GameObject::SetScale(Vector3 scale)
{
    if(m_transform)
        m_transform->SetScale(scale);
}

void GameObject::AddComponent(Component* comp)
{
    m_components.push_back(comp);
}
}
