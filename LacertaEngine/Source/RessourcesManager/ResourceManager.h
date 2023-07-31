#pragma once
#include "../Core.h"
#include "Resource.h"
#include "../Logger/Logger.h"


namespace LacertaEngine
{
    
class LACERTAENGINE_API ResourceManager
{
private:
    ResourceManager();
    ~ResourceManager();

public:
    static ResourceManager* Get();
    static void Create();
    static void Shutdown();

    template <typename T>
    T* CreateResource(const wchar_t* filePath)
    {
        LOG(Debug, "Resource Manager : Create Resource");

        for(auto* resource : m_resources)
        {
            if(filePath == resource->GetFilePath())
                return dynamic_cast<T*>(resource);
        }

        T* resource = new T();
        Resource* res = static_cast<Resource*>(resource);
        if(!res)
        {
            LOG(Error, "Resource Manager : Wrong template argument in CreateResource");
            return nullptr;
        }
        res->CreateResource(filePath);

        return resource;
    }

    std::list<Resource*> GetResources() { return m_resources; }

private:
    std::list<Resource*> m_resources;
    static ResourceManager* s_resourceManager;
};
    
}
