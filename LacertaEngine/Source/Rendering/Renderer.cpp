#include "Renderer.h"
#include "Drawcall.h"
#include "Shader.h"

namespace LacertaEngine
{
    
Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::ClearDrawcalls()
{
    for(auto dc : m_drawcalls)
    {
        delete dc;
    }

    m_drawcalls.clear();
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
