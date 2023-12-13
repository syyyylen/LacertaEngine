﻿#pragma once
#include <map>

#include "../Core.h"
#include "../Maths/Maths.h"

namespace LacertaEngine
{
    
class Drawcall;
struct DrawcallData;
class RenderTarget;
class Shader;
class Mesh;
struct VertexMesh;
struct ShapeData;

class LACERTAENGINE_API Renderer
{
public:
    Renderer();
    virtual ~Renderer();

    virtual void Initialize(int* context, int width, int height, int targetRefreshRate) = 0;
    virtual void CreateRenderTarget(int width, int height, int depth) = 0;
    virtual void LoadShaders() = 0;
    virtual void RenderFrame(Vector2 ViewportSize) = 0;
    virtual void OnResizeWindow(unsigned width, unsigned height) = 0;
    virtual void PresentSwapChain() = 0;
    virtual void UpdateConstantBuffer(void* buffer) = 0;
    virtual void UpdateMeshConstantBuffer(void* buffer) = 0;
    virtual void SetRasterizerCullState(bool cullFront) = 0;
    virtual void AddDrawcall(DrawcallData* dcData) = 0;
    virtual void ClearDrawcalls();
    virtual void CreateBuffers(ShapeData& shapeData, std::vector<VertexMesh> vertices, std::vector<unsigned int> indices) = 0;

    virtual int* GetDriver() = 0;
    
    RenderTarget* GetRenderTarget(int index);

    bool HasShader(std::string shaderName) { return m_shaders.find(shaderName) != m_shaders.end(); }
    Shader* GetShader(std::string shaderName);

protected:
    std::vector<RenderTarget*> m_renderTargets;
    std::list<Drawcall*> m_drawcalls;
    std::map<std::string, Shader*> m_shaders;
};

}
