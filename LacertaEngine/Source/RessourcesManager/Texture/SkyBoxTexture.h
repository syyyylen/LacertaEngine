#pragma once
#include "../Resource.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API SkyBoxTexture : public Resource
{
public:
    SkyBoxTexture();
    ~SkyBoxTexture();

    void CreateResource(const wchar_t* filePath) override;

    void* m_resource = nullptr;
    void* m_resourceView = nullptr;
};
    
}
