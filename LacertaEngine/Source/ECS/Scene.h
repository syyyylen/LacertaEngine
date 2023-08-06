#pragma once
#include "../Core.h"
#include "Entt/entt.hpp"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Scene
{
public:
    Scene();
    ~Scene();

private:
    entt::registry m_registry;
};
    
}
