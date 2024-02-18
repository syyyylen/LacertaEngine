﻿#include "RenderPass.h"
#include "Drawcall.h"
#include "Bindable.h"
#include "Renderer.h"
#include "RenderTarget.h"

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

void RenderPass::AddDrawcall(std::string shaderName, Drawable* drawable, std::list<Bindable*> bindables)
{
    auto dc = new Drawcall(shaderName, drawable, bindables);
    m_drawcalls.push_back(dc);
}

void RenderPass::Pass(Renderer* renderer, Vector2 renderTargetSize)
{
    auto RT = renderer->GetRenderTarget(m_renderTargetIdx);
    RT->SetViewportSize(renderer, (UINT)renderTargetSize.X, (UINT)renderTargetSize.Y);
    if(m_cachedRenderTargetSize.X != renderTargetSize.X || m_cachedRenderTargetSize.Y != renderTargetSize.Y)
    {
        RT->Resize(renderer, renderTargetSize.X, renderTargetSize.Y);
        m_cachedRenderTargetSize = renderTargetSize;
    }

    RT->SetActive(renderer);
    RT->Clear(renderer, Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    
    for(auto bindable : m_globalBindables)
        bindable->Bind(renderer);
    
    for(auto dc : m_drawcalls)
    {
        dc->PreparePass(renderer);
        dc->Pass(renderer);
    }
}

void RenderPass::ClearDrawcalls()
{
    for(auto dc : m_drawcalls)
        delete dc;

    m_drawcalls.clear();
}

}