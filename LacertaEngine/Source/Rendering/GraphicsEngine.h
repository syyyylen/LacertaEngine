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
public:
    GraphicsEngine();
    ~GraphicsEngine();
    
    void InitializeRenderer(int* context, RendererType type, int width, int height, int depth, int targetRefreshRate);
    void Render();
    void PresentSwapChain();
    void Shutdown();
    
    Renderer* GetRenderer();

private:
    RendererType m_rendererType;

public:
    Renderer* m_renderer;
};
    
}


