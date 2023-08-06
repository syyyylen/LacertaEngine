#pragma once
#include "Entt/entt.hpp"
#include "Scene.h"
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API GameObject
{
public:
    GameObject();
    GameObject(Scene* scene, entt::entity entityHandle, std::string name);
    ~GameObject();

    template<typename T, typename... Args>
    T& AddComponent(Args&&... args)
    {
        return m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& GetComponent()
    {
        return m_scene->m_registry.get<T>(m_entityHandle);
    }

    template<typename T>
    bool HasComponent()
    {
        return m_scene->m_registry.all_of<T>(m_entityHandle);
    }

    template<typename T>
    void RemoveComponent()
    {
        m_scene->m_registry.remove<T>(m_entityHandle);
    }
    
private:
    entt::entity m_entityHandle;
    std::string m_name;
    Scene* m_scene;
};
    
}
