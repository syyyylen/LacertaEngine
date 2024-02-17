#pragma once

#include "GraphicsResource.h"
#include "Renderer.h"
#include "Texture.h"
#include "../Core.h"
#include "../Logger/Logger.h"

namespace LacertaEngine
{

struct VertexMesh;
    
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

    void InitializeRenderer(int* context, RendererType type, int width, int height, int depth, int targetRefreshRate);
    void AddDrawcall(DrawcallData* dcData);
    void ClearDrawcalls();
    void RenderScene(Vector2 ViewportSize);
    void Resize(unsigned width, unsigned height);
    void SetBackbufferViewportSize(int width, int height);
    void SetRasterizerState(bool cullFront);
    void PresentSwapChain();
    void UpdateShaderConstants(void* buffer);
    void UpdateMeshConstants(void* buffer);

    void CreateBuffers(ShapeData& mesh, std::vector<VertexMesh> vertices, std::vector<unsigned int> indices);

    Renderer* GetRenderer() { return m_renderer; }
    RendererType GetRendererType() { return m_rendererType; }

    std::list<GraphicsResource*> GetGraphicsResources;

    Mesh* CreateMesh(const wchar_t* filePath);
    Texture* CreateTexture(const wchar_t* filePath);

private:
    RendererType m_rendererType;
    Renderer* m_renderer;

    static RHI* s_graphicsEngine;
};
    
}


