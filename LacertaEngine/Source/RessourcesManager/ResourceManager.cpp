#include "ResourceManager.h"
#include "../Rendering/GraphicsEngine.h"
#include "Texture/DX11Texture.h"

namespace LacertaEngine
{

ResourceManager* ResourceManager::s_resourceManager = nullptr;
    
ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
    s_resourceManager = nullptr;
}

ResourceManager* ResourceManager::Get()
{
    return s_resourceManager;
}

void ResourceManager::Create()
{
    LOG(Debug, "Resource Manager : Create");
    
    if(s_resourceManager)
        throw std::exception("Resource Manager already created");

    s_resourceManager = new ResourceManager();
}

void ResourceManager::Shutdown()
{
    LOG(Debug, "Resource Manager : Shutdown");
}

Texture* ResourceManager::CreateTexture(const wchar_t* filePath)
{
    RendererType rdrType = GraphicsEngine::Get()->GetRendererType();

    switch (rdrType)
    {
    case RENDERER_WIN_DX11:
        {
            DX11Texture* Texture = CreateResource<DX11Texture>(filePath);
            return Texture;
        }
    }

    return nullptr;
}
}

