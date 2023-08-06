#include "GameObject.h"

namespace LacertaEngine
{
    
GameObject::GameObject()
{
}

GameObject::GameObject(Scene* scene, entt::entity entityHandle, std::string name) : m_scene(scene), m_entityHandle(entityHandle), m_name(name)
{
}

GameObject::~GameObject()
{
}

// void GameObject::SetPosition(Vector3 position)
// {
//     if(m_transform)
//         m_transform->SetPosition(position);
// }
//
// void GameObject::SetScale(Vector3 scale)
// {
//     if(m_transform)
//         m_transform->SetScale(scale);
// }
//
// void GameObject::AddComponent(Component* comp)
// {
//     m_components.push_back(comp);
// }
    
}
