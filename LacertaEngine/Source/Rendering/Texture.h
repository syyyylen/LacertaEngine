#pragma once
#include "GraphicsResource.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Texture : public GraphicsResource
{
public:
    Texture();
    ~Texture();

    virtual void CreateResource(const wchar_t* filePath, Renderer* renderer) = 0;

    // TODO remove me
    virtual void* GetResource() = 0;
    virtual void* GetResourceView() = 0;
};
    
}

