#include "WinDX11Drawcall.h"

#include "WinDX11Renderer.h"
#include "WinDX11Shader.h"
#include "../../Logger/Logger.h"
#include "../Material.h"

namespace LacertaEngine
{
    
WinDX11Drawcall::WinDX11Drawcall()
{
}

WinDX11Drawcall::~WinDX11Drawcall()
{
    // vbo owner is the mesh itself, we just store the pointer inside the dc
    // if(m_vbo != nullptr)
    // {
    //     m_vbo->Release();
    //     m_vbo = nullptr;
    // }
}

void WinDX11Drawcall::Setup(Renderer* renderer, DrawcallData* dcData)
{
    m_vbo = static_cast<ID3D11Buffer*>(dcData->VBO);
    m_verticesCount = dcData->VerticesCount;
    m_ibo = static_cast<ID3D11Buffer*>(dcData->IBO);
    m_indexCount = dcData->IndicesCount;

    // TODO the way the info is transferred all across the pipeline seems stupid af 
    m_type = dcData->Type;
    if(dcData->Type == DrawcallType::dcMesh)
        m_localMatrix = dcData->LocalMatrix;

    m_material = dcData->Material;

    if(!renderer->HasShader(m_material->GetShader()))
    {
        LOG(Error, "Invalid Shader Name, not contained in the current renderer");
        return;
    }
    
    m_shader = renderer->GetShader(m_material->GetShader());
    m_shader->Load(renderer, dcData->Type);
}

void WinDX11Drawcall::Pass(Renderer* renderer)
{
    m_shader->PreparePass(renderer, this);
    m_shader->Pass(renderer, this);
}

}
