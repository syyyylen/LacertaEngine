#pragma once
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API RenderTarget
{
public:
    RenderTarget();
    virtual ~RenderTarget();

    virtual void Initialize() = 0;
    virtual void SetActive() = 0;
    virtual void Clear() = 0;

private:
    int m_width;
    int m_length;
};

}
