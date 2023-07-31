#include "ResourceManager.h"

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

}

