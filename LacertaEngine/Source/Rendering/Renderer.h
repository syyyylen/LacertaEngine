#pragma once
#include <map>

#include "GraphicsResource.h"
#include "../Core.h"
#include "../Maths/Maths.h"

namespace LacertaEngine
{
class Drawcall;
struct DrawcallData;
class RenderTarget;
class Shader;
class Mesh;
class Texture;
struct SceneVertexMesh;
class Drawable;
class Bindable;
class RenderPass;
enum ConstantBufferType;

class LACERTAENGINE_API Renderer
{
public:
    Renderer();
    virtual ~Renderer();

    virtual void Initialize(int* context, int width, int height, int targetRefreshRate) = 0;
    virtual void CreateRenderTarget(int width, int height) = 0;
    virtual void LoadShaders() = 0;
    virtual void SetBackbufferRenderTargetActive() = 0;
    virtual void OnResizeWindow(unsigned width, unsigned height) = 0;
    virtual void PresentSwapChain() = 0;
    virtual void UpdateConstantBuffer(void* buffer, ConstantBufferType cbufType) = 0;
    virtual void SetRasterizerCullState(bool cullFront) = 0;
    RenderPass* CreateRenderPass(std::string name);
    RenderPass* GetRenderPass(std::string name);
    void DeleteRenderPass(std::string name);
    void ExecuteRenderPass(std::string name, Vector2 renderTargetSize);
    
    virtual Mesh* CreateMesh(const wchar_t* filePath) = 0;
    virtual Texture* CreateTexture(const wchar_t* filePath, int idx) = 0;

    virtual int* GetDriver() = 0;

    RenderTarget* GetRenderTarget(int index);

    bool HasShader(std::string shaderName) { return m_shaders.find(shaderName) != m_shaders.end(); }
    Shader* GetShader(std::string shaderName);

protected:
    std::vector<RenderTarget*> m_renderTargets;
    std::list<Drawcall*> m_drawcalls;
    std::map<std::string, Shader*> m_shaders;
    std::list<GraphicsResource*> m_graphicsResources;
    std::map<std::string, RenderPass*> m_renderPasses;
};
}