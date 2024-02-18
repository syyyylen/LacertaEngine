#include "Renderer.h"
#include "Drawcall.h"
#include "RenderPass.h"
#include "Shader.h"

namespace LacertaEngine
{
    
Renderer::Renderer()
{
}

Renderer::~Renderer()
{
    for(auto shader : m_shaders)
        delete shader.second;

    for(auto res : m_graphicsResources)
        delete res;
}

RenderPass* Renderer::CreateRenderPass(std::string name)
{
    auto pass = new RenderPass();
    m_renderPasses.emplace(name, pass);
    return pass;
}

RenderPass* Renderer::GetRenderPass(std::string name)
{
    return m_renderPasses.find(name)->second;
}

void Renderer::DeleteRenderPass(std::string name)
{
    auto pass = m_renderPasses.find(name)->second;
    m_renderPasses.erase(name);
    delete pass;
}

void Renderer::ExecuteRenderPass(std::string name, Vector2 renderTargetSize)
{
    auto pass = m_renderPasses.find(name)->second;
    pass->Pass(this, renderTargetSize);
}

RenderTarget* Renderer::GetRenderTarget(int index)
{
    if(m_renderTargets.size() > index)
        return m_renderTargets[index];

    return nullptr;
}

Shader* Renderer::GetShader(std::string shaderName)
{
    auto it = m_shaders.find(shaderName);
    if (it != m_shaders.end()) {
        return it->second;
    }
    return nullptr; 
}
    
}
