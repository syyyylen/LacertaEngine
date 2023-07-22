#pragma once
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Shader
{
public:
    Shader();
    virtual ~Shader();

    virtual void Load() = 0;
    virtual void PreparePass() = 0;
    virtual void Pass() = 0;
};

}
