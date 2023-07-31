#include "WinDX11Drawcall.h"

#include "WinDX11Renderer.h"
#include "WinDX11Shader.h"
#include "../../Logger/Logger.h"

namespace LacertaEngine
{
    
WinDX11Drawcall::WinDX11Drawcall()
{
}

WinDX11Drawcall::~WinDX11Drawcall()
{
    if(m_vbo)
    {
        m_vbo->Release();
        delete m_vbo;
    }
}

void WinDX11Drawcall::Setup(Renderer* renderer, DrawcallData* dcData)
{
    LOG(Debug, "WinDX11Shader : Setup");

    m_type = dcData->Type;
    if(dcData->Type == DrawcallType::Mesh)
        m_localMatrix = dcData->LocalMatrix;
    
    auto shader = new WinDX11Shader();
    m_shader = (Shader*)shader;
    m_shader->Load(renderer, dcData->Type, dcData->VertexShaderPath, dcData->PixelShaderPath);
}

void WinDX11Drawcall::Pass(Renderer* renderer)
{
    m_shader->PreparePass(renderer, this);
    m_shader->Pass(renderer, this);
}

void WinDX11Drawcall::CreateVBO(Renderer* renderer, void* data, unsigned long size)
{
    LOG(Debug, "WinDX11Shader : CreateVBO");

    if(m_vbo)
        m_vbo->Release();

    unsigned long dataLength = size * (unsigned long)((WinDX11Shader*)m_shader)->GetVerticesStride();

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = dataLength;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = data;

    ID3D11Device* dev = (ID3D11Device*)renderer->GetDriver();

    HRESULT hr = dev->CreateBuffer(&desc, &InitData, &m_vbo);

    if (FAILED(hr))
    {
        LOG(Error, "WinDX11Drawcall : VBO object creation failed");
        throw std::exception("VBO object creation failed");
    }

    m_verticesCount = size;
}

void WinDX11Drawcall::CreateIBO(Renderer* renderer, void* data, unsigned long size)
{
    LOG(Debug, "WinDX11Shader : CreateIBO");

    if(m_ibo)
        m_ibo->Release();

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = 4 * size;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = data;

    ID3D11Device* dev = (ID3D11Device*)renderer->GetDriver();

    HRESULT hr = dev->CreateBuffer(&desc, &InitData, &m_ibo);

    if (FAILED(hr))
    {
        LOG(Error, "WinDX11Drawcall : IBO object creation failed");
        throw std::exception("IBO object creation failed");
    }

    m_indexCount = size;
}
    
}
