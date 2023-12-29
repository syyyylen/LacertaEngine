#pragma once
#include "../Core.h"

namespace LacertaEngine
{
    class Scene;
}

namespace LacertaEngine
{
    
class LACERTAENGINE_API SceneSerializer
{
public:
    SceneSerializer();
    ~SceneSerializer();

    bool Serialize(Scene& Scene, const wchar_t* filePath);
    bool Deserialize(Scene& Scene, const wchar_t* filePath);
};

}

