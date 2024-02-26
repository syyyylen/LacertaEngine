#pragma once

#include "GraphicsResource.h"
#include "Renderer.h"
#include "../Core.h"

namespace LacertaEngine
{

struct SceneVertexMesh;
    
enum RendererType
{
    RENDERER_WIN_DX11
};

class LACERTAENGINE_API RHI
{
private:
    RHI();
    ~RHI();

public:
    static RHI* Get();
    static void Create();
    static void Shutdown();

    void InitializeRenderer(int* context, RendererType type, int width, int height, int targetRefreshRate);
    void SetBackbufferRenderTargetActive();
    void Resize(unsigned width, unsigned height);
    void SetBackbufferViewportSize(int width, int height);
    void SetRasterizerState(bool cullFront);
    void PresentSwapChain();
    RenderTarget* CreateRenderTarget(int width, int height, int& outRTidx);
    RenderTarget* GetRenderTarget(int idx);
    RenderPass* CreateRenderPass(std::string name);
    RenderPass* GetRenderPass(std::string name);
    void DeleteRenderPass(std::string name);
    void ExecuteRenderPass(std::string name, Vector2 renderTargetSize, bool clear);

    Renderer* GetRenderer() { return m_renderer; }
    RendererType GetRendererType() { return m_rendererType; }
    
    Mesh* CreateMesh(const wchar_t* filePath);
    Texture* CreateTexture(const wchar_t* filePath, int idx);

private:
    RendererType m_rendererType;
    Renderer* m_renderer;

    static RHI* s_graphicsEngine;
};
    
}


