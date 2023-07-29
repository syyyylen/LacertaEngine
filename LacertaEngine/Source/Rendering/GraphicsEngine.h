#pragma once

#include "Renderer.h"
#include "../Core.h"

namespace LacertaEngine
{
    
enum RendererType
{
    RENDERER_WIN_DX11
};

class LACERTAENGINE_API GraphicsEngine
{
private:
    GraphicsEngine();
    ~GraphicsEngine();

public:
    static GraphicsEngine* Get();
    static void Create();
    static void Shutdown();
    
    void InitializeRenderer(int* context, RendererType type, int width, int height, int depth, int targetRefreshRate);
    void Render();
    void Resize(unsigned width, unsigned height);
    void PresentSwapChain();
    Renderer* GetRenderer() { return m_renderer; }
    
private:
    RendererType m_rendererType;
    Renderer* m_renderer;

    static GraphicsEngine* s_graphicsEngine;
};
    
}


