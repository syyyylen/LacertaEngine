#include "RenderPass.h"
#include "Drawcall.h"
#include "Bindable.h"
#include "Renderer.h"
#include "RenderTarget.h"
#include "RHI.h"

namespace LacertaEngine
{
    
RenderPass::RenderPass()
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::AddGlobalBindable(Bindable* bindable)
{
    m_globalBindables.emplace_back(bindable);
}

void RenderPass::AddDrawcall(std::string shaderName, Drawable* drawable, std::vector<Bindable*> bindables)
{
    auto dc = new Drawcall(shaderName, drawable, bindables);
    m_drawcalls.push_back(dc);
}

void RenderPass::Pass(Renderer* renderer, Vector2 renderTargetSize, bool clear)
{
    auto RT = renderer->GetRenderTarget(m_renderTargetIdx);
    if(RT->GetRenderTargetType() != RenderTargetType::TextureCube) // for texture cubes (multiple RT) user need to set active and clear by hand
    {
        RT->SetViewportSize(renderer, (UINT)renderTargetSize.X, (UINT)renderTargetSize.Y);
        if(m_cachedRenderTargetSize.X != renderTargetSize.X || m_cachedRenderTargetSize.Y != renderTargetSize.Y)
        {
            RT->Resize(renderer, renderTargetSize.X, renderTargetSize.Y);
            m_cachedRenderTargetSize = renderTargetSize;
        }

        RT->SetActive(renderer);
        if(clear) // TODO remove this, this has nothing to do here
            RT->Clear(renderer, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    }
    
    RHI::Get()->SetRasterizerState(m_cullfront);
    
    for(auto bindable : m_globalBindables)
        bindable->Bind(renderer);
    
    for(auto dc : m_drawcalls)
    {
        dc->PreparePass(renderer);
        dc->Pass(renderer);
    }
}

void RenderPass::SetCullfront(bool state)
{
    m_cullfront = state;
}

void RenderPass::ClearDrawcalls()
{
    for(auto dc : m_drawcalls)
        delete dc;

    m_drawcalls.clear();
}

void RenderPass::ClearGlobalBindables()
{
    m_globalBindables.clear();
}
}
