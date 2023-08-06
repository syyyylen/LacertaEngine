#pragma once
#include <map>

#include "../Core.h"

namespace LacertaEngine
{
    
class Drawcall;
struct DrawcallData;
class RenderTarget;
class Shader;
class Mesh;
struct VertexMesh;

class LACERTAENGINE_API Renderer
{
public:
    Renderer();
    virtual ~Renderer();

    virtual void Initialize(int* context, int width, int height, int targetRefreshRate) = 0;
    virtual void CreateRenderTarget(int width, int height, int depth) = 0;
    virtual void LoadShaders() = 0;
    virtual void RenderFrame() = 0;
    virtual void OnResize(unsigned width, unsigned height) = 0;
    virtual void PresentSwapChain() = 0;
    virtual void UpdateConstantBuffer(void* buffer) = 0;
    virtual void UpdateMeshConstantBuffer(void* buffer) = 0;
    virtual void AddDrawcall(DrawcallData* dcData) = 0;
    virtual void ClearDrawcalls();
    virtual void CreateBuffers(Mesh* mesh, std::vector<VertexMesh> vertices, std::vector<unsigned int> indices) = 0;

    virtual int* GetDriver() = 0;
    
    RenderTarget* GetRenderTarget() { return m_renderTarget; }

    bool HasShader(std::string shaderName) { return m_shaders.find(shaderName) != m_shaders.end(); }
    Shader* GetShader(std::string shaderName);

protected:
    RenderTarget* m_renderTarget;
    std::list<Drawcall*> m_drawcalls;
    std::map<std::string, Shader*> m_shaders;
};

}
