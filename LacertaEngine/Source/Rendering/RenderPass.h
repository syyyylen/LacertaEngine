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
    void SetRenderTargetSubresourceIdx(int idx) { m_renderTargetSubresourceIdx = idx; }
    void AddGlobalBindable(Bindable* bindable);
    void AddDrawcall(std::string shaderName, Drawable* drawable, std::vector<Bindable*> bindables);
    void Pass(Renderer* renderer, Vector2 renderTargetSize, bool clear);
    void SetCullfront(bool state);
    void SetComparisonSampling(bool compare);
    void ClearDrawcalls();
    void ClearGlobalBindables();

private:
    std::vector<Bindable*> m_globalBindables;
    std::vector<Drawcall*> m_drawcalls;
    int m_renderTargetIdx;
    int m_renderTargetSubresourceIdx = 0;
    Vector2 m_cachedRenderTargetSize;
    bool m_cullfront = false;
    bool m_comparisonSampling = false;
};

}