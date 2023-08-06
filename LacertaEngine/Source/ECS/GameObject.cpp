#include "GameObject.h"

namespace LacertaEngine
{
    
GameObject::GameObject()
{
}

GameObject::GameObject(Scene* scene, entt::entity entityHandle, std::string name, Vector3 position) :
    m_scene(scene), m_entityHandle(entityHandle), m_name(name), m_startPosition(position)
{
}

GameObject::~GameObject()
{
}

}
