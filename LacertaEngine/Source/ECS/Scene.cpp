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

GameObject* Scene::CreateGameObject(std::string name, Vector3 position)
{
    entt::entity entityHandle = m_registry.create();
    GameObject* go = new GameObject(this, entityHandle, name, position);
    auto& tfComp = go->AddComponent<TransformComponent>();
    tfComp.SetStartPosition(position);
    tfComp.SetPosition(position);
    return go;
}

}
