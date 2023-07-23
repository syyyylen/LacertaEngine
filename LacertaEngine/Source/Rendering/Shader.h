#pragma once
#include "Renderer.h"
#include "Drawcall.h"
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Shader
{
public:
    Shader();
    virtual ~Shader();

    virtual void Load(Renderer* renderer) = 0;
    virtual void PreparePass(Renderer* renderer, Drawcall* dc) = 0;
    virtual void Pass(Renderer* renderer, Drawcall* dc) = 0;
};

}
