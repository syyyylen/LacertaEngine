#include "Drawcall.h"
#include "Shader.h"
#include "../Logger/Logger.h"

namespace LacertaEngine
{
    
Drawcall::Drawcall(std::string shaderName, Drawable* drawable, std::list<Bindable*> bindables)
    : m_shaderName(shaderName), m_shader(nullptr), m_drawable(drawable), m_bindables(bindables)
{
}

Drawcall::~Drawcall()
{
}

void Drawcall::PreparePass(Renderer* renderer)
{
    m_shader = renderer->GetShader(m_shaderName);
    m_shader->Load(renderer, m_drawable->GetLayout());
    
    for(auto bindable : m_bindables)
        bindable->Bind(renderer);

    m_drawable->BindBuffers(renderer);
}

void Drawcall::Pass(Renderer* renderer)
{
    m_shader->PreparePass(renderer);
    m_shader->Pass(renderer, this);
}

}
