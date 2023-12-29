#include "SceneSerializer.h"
#include "Scene.h"
#include "../Logger/Logger.h"

namespace LacertaEngine
{
    SceneSerializer::SceneSerializer()
    {
    }

    SceneSerializer::~SceneSerializer()
    {
    }

    bool SceneSerializer::Serialize(Scene& Scene, const wchar_t* filePath)
    {
        LOG(Warning, "Serializing : " + Scene.m_name);
        return false;
    }

    bool SceneSerializer::Deserialize(Scene& Scene, const wchar_t* filePath)
    {
        return false;
    }
}
