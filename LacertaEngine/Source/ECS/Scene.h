﻿#pragma once
#include "../Core.h"
#include "Entt/entt.hpp"

namespace LacertaEngine
{
    class Vector3;

    class GameObject;

class LACERTAENGINE_API Scene
{
public:
    Scene(std::string name);
    ~Scene();

    GameObject* CreateGameObject(std::string name, Vector3 position);
    void RemoveGameObject(GameObject* goToRemove);

public:
    entt::registry m_registry;
    std::vector<GameObject*> m_gameObjects;

private:
    std::string m_name;

    friend class SceneSerializer;
};
    
}
