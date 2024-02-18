#pragma once
#include "../Core.h"

namespace LacertaEngine
{

class Renderer;
    
class LACERTAENGINE_API Bindable
{
public:
    Bindable();
    virtual ~Bindable();

    virtual void Bind(Renderer* renderer) = 0;
};

}