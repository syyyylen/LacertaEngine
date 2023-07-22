#include "GraphicsEngine.h"

#include "../Logger/Logger.h"
#include "WinDX11/WinDX11Renderer.h"

LacertaEngine::GraphicsEngine::GraphicsEngine()
{
}

LacertaEngine::GraphicsEngine::~GraphicsEngine()
{
}

void LacertaEngine::GraphicsEngine::InitializeRenderer(int* context, RendererType type, int width, int height, int depth, int targetRefreshRate)
{
    LOG(Debug, "GraphicsEngine : Initialize Renderer");

    m_rendererType = type;

    switch (m_rendererType)
    {
        case RendererType::RENDERER_WIN_DX11:
            m_renderer = new WinDX11Renderer();
            break;
    }

    if(m_renderer)
    {
        m_renderer->Initialize(context, width, height, targetRefreshRate);
        m_renderer->CreateRenderTarget(width, height, depth);
    }
}

void LacertaEngine::GraphicsEngine::Render()
{
    m_renderer->RenderFrame();
}

void LacertaEngine::GraphicsEngine::Shutdown()
{
    LOG(Debug, "GraphicsEngine : Shutdown");
    
}
