#pragma once
#include "../Resource.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Texture : public Resource
{
public:
    Texture();
    ~Texture();

    void CreateResource(const wchar_t* filePath) override;
};
    
}


