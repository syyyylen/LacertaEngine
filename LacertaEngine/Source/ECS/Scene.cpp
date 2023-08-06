#include "Scene.h"
#include "GameObject.h"
#include "Components/TransformComponent.h"

namespace LacertaEngine
{
    Scene::Scene()
    {
       
    }

    Scene::~Scene()
    {
        
    }

    GameObject* Scene::CreateGameObject(std::string name)
    {
        entt::entity entityHandle = m_registry.create();
        GameObject* go = new GameObject(this, entityHandle, name);
        go->AddComponent<TransformComponent>();
        return go;
    }
}
