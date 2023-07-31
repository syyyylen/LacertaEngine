#include "GraphicsEngine.h"

#include "../Logger/Logger.h"
#include "WinDX11/WinDX11Renderer.h"
#include "Drawcall.h"

namespace LacertaEngine
{

GraphicsEngine* GraphicsEngine::s_graphicsEngine = nullptr;
    
GraphicsEngine::GraphicsEngine()
{
}

GraphicsEngine::~GraphicsEngine()
{
    s_graphicsEngine = nullptr;
}

GraphicsEngine* GraphicsEngine::Get()
{
    return s_graphicsEngine;
}

void GraphicsEngine::Create()
{
    if(s_graphicsEngine)
        throw std::exception("Graphics Engine already created");

    s_graphicsEngine = new GraphicsEngine();
}

void GraphicsEngine::InitializeRenderer(int* context, RendererType type, int width, int height, int depth, int targetRefreshRate)
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
        m_renderer->CreateRenderTarget(width, height, depth);
    }
}

void GraphicsEngine::AddDrawcall(DrawcallData* dcData)
{
    if(m_renderer)
        m_renderer->AddDrawcall(dcData);
}

void GraphicsEngine::Render()
{
    if(m_renderer)
        m_renderer->RenderFrame();
}

void GraphicsEngine::Resize(unsigned width, unsigned height)
{
    if(m_renderer)
        m_renderer->OnResize(width, height);
}

void GraphicsEngine::PresentSwapChain()
{
    if(m_renderer)
        m_renderer->PresentSwapChain();
}

void GraphicsEngine::UpdateShaderConstants(void* buffer)
{
    if(m_renderer)
        m_renderer->UpdateConstantBuffer(buffer);
}

void GraphicsEngine::UpdateMeshConstants(void* buffer)
{
    if(m_renderer)
        m_renderer->UpdateMeshConstantBuffer(buffer);
}

void GraphicsEngine::Shutdown()
{
    LOG(Debug, "GraphicsEngine : Shutdown");

    if(!s_graphicsEngine)
        return;
    
    delete s_graphicsEngine;
}
    
}
