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
    for(auto go : m_gameObjects)
        delete go;
}

GameObject* Scene::CreateGameObject(std::string name, Vector3 position)
{
    entt::entity entityHandle = m_registry.create();
    
    GameObject* go = new GameObject(this, entityHandle, name, position);
    
    auto& tfComp = go->AddComponent<TransformComponent>();
    tfComp.SetStartPosition(position);
    tfComp.SetPosition(position);

    m_gameObjects.emplace_back(go);
    
    return go;
}

void Scene::RemoveGameObject(GameObject* goToRemove)
{
    m_gameObjects.erase(std::remove(m_gameObjects.begin(), m_gameObjects.end(), goToRemove), m_gameObjects.end());
    m_registry.destroy(goToRemove->GetEntityHandle());
    delete goToRemove;
}
    
}
