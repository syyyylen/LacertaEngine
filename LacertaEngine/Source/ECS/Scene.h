#pragma once
#include "../Core.h"
#include "Entt/entt.hpp"

namespace LacertaEngine
{

class GameObject;

class LACERTAENGINE_API Scene
{
public:
    Scene();
    ~Scene();

    GameObject* CreateGameObject(std::string name);

public:
    entt::registry m_registry;
};
    
}
