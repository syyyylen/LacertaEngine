#pragma once
#include "../Core.h"

namespace LacertaEngine
{

class LACERTAENGINE_API Drawcall
{
public:
    Drawcall();
    virtual ~Drawcall();

    virtual void Setup() = 0;
    virtual void Pass() = 0;

protected:
    
};

}
