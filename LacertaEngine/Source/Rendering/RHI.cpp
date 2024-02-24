#include "RHI.h"

#include "../Logger/Logger.h"
#include "WinDX11/WinDX11Renderer.h"
#include "Drawcall.h"
#include "RenderTarget.h"

namespace LacertaEngine
{

RHI* RHI::s_graphicsEngine = nullptr;
    
RHI::RHI()
{
}

RHI::~RHI()
{
    s_graphicsEngine = nullptr;
}

RHI* RHI::Get()
{
    return s_graphicsEngine;
}

void RHI::Create()
{
    if(s_graphicsEngine)
        throw std::exception("Graphics Engine already created");

    s_graphicsEngine = new RHI();
}

void RHI::InitializeRenderer(int* context, RendererType type, int width, int height, int targetRefreshRate)
{
    LOG(Debug, "GraphicsEngine : Initialize Renderer");

    m_rendererType = type;

    switch (m_rendererType)
    {
        case RENDERER_WIN_DX11:
            m_renderer = new WinDX11Renderer();
            break;
    }

    if(m_renderer)
    {
        m_renderer->Initialize(context, width, height, targetRefreshRate);
        m_renderer->CreateRenderTarget(width, height);
        m_renderer->LoadShaders();
    }
}

void RHI::SetBackbufferRenderTargetActive() // TODO fix all this
{
    if(m_renderer)
        m_renderer->SetBackbufferRenderTargetActive();
}

void RHI::Resize(unsigned width, unsigned height)
{
    if(m_renderer)
        m_renderer->OnResizeWindow(width, height);
}

void RHI::SetBackbufferViewportSize(int width, int height)
{
    if(m_renderer)
        m_renderer->GetRenderTarget(0)->SetViewportSize(m_renderer, width, height);
}

void RHI::SetRasterizerState(bool cullFront)
{
    if(m_renderer)
        m_renderer->SetRasterizerCullState(cullFront);
}

void RHI::PresentSwapChain()
{
    if(m_renderer)
        m_renderer->PresentSwapChain();
}

RenderPass* RHI::CreateRenderPass(std::string name)
{
    return m_renderer->CreateRenderPass(name);
}

RenderPass* RHI::GetRenderPass(std::string name)
{
    return m_renderer->GetRenderPass(name);
}

void RHI::DeleteRenderPass(std::string name)
{
    m_renderer->DeleteRenderPass(name);
}

void RHI::ExecuteRenderPass(std::string name, Vector2 renderTargetSize, bool clear)
{
    m_renderer->ExecuteRenderPass(name, renderTargetSize, clear);
}

Mesh* RHI::CreateMesh(const wchar_t* filePath)
{
    if(m_renderer)
        return m_renderer->CreateMesh(filePath);

    return nullptr;
}

Texture* RHI::CreateTexture(const wchar_t* filePath, int idx)
{
    if(m_renderer)
        return m_renderer->CreateTexture(filePath, idx);

    return nullptr;
}

void RHI::Shutdown()
{
    LOG(Debug, "GraphicsEngine : Shutdown");

    if(!s_graphicsEngine)
        return;
    
    delete s_graphicsEngine;
}
}
