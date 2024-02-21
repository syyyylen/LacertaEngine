#pragma once
#include "../Core.h"

namespace LacertaEngine
{

class Renderer;
enum DrawableLayout;
    
class LACERTAENGINE_API Drawable
{
public:
    Drawable();
    virtual ~Drawable();

    virtual void BindBuffers(Renderer* renderer) = 0;
    virtual DrawableLayout GetLayout() = 0;
    virtual unsigned long GetVerticesSize() = 0;
    virtual unsigned long GetIndicesSize() = 0;
};

}