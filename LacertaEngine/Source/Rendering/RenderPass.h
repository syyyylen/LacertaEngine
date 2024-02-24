#pragma once
#include "../Core.h"
#include "../Maths/Maths.h"

namespace LacertaEngine
{

class Bindable;
class Drawcall;
class Drawable;
class Renderer;
    
class LACERTAENGINE_API RenderPass
{
public:
    RenderPass();
    ~RenderPass();

    void SetRenderTargetIdx(int idx) { m_renderTargetIdx = idx; }
    void AddGlobalBindable(Bindable* bindable);
    void AddDrawcall(std::string shaderName, Drawable* drawable, std::list<Bindable*> bindables);
    void Pass(Renderer* renderer, Vector2 renderTargetSize, bool clear);
    void SetCullfront(bool state);
    void ClearDrawcalls();
    void ClearGlobalBindables();

private:
    std::list<Bindable*> m_globalBindables;
    std::list<Drawcall*> m_drawcalls;
    int m_renderTargetIdx;
    Vector2 m_cachedRenderTargetSize;
    bool m_cullfront = false;
};

}