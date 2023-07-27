#include "WinDX11Drawcall.h"

#include "WinDX11Renderer.h"
#include "WinDX11Shader.h"
#include "../../Logger/Logger.h"

LacertaEngine::WinDX11Drawcall::WinDX11Drawcall()
{
}

LacertaEngine::WinDX11Drawcall::~WinDX11Drawcall()
{
    if(m_vbo)
    {
        m_vbo->Release();
        delete m_vbo;
    }
}

void LacertaEngine::WinDX11Drawcall::Setup(Renderer* renderer)
{
    LOG(Debug, "WinDX11Shader : Setup");
    
    auto shader = new WinDX11Shader();
    m_shader = (Shader*)shader;

    m_shader->Load(renderer, L"ScreenVertex.hlsl", L"SimpleColorPixelShader.hlsl");
}

void LacertaEngine::WinDX11Drawcall::Pass(Renderer* renderer)
{
    m_shader->PreparePass(renderer, this);
    m_shader->Pass(renderer, this);
}

void LacertaEngine::WinDX11Drawcall::CreateVBO(Renderer* renderer, void* data, unsigned long size)
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
    
    /*
    
    unsigned long dataLength = size * (unsigned long)((WinDX11Shader*)m_shader)->GetVerticesStride();

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.ByteWidth = dataLength;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    ID3D11Device* dev = (ID3D11Device*)renderer->GetDriver();

    HRESULT hr = dev->CreateBuffer(&desc, NULL, &m_vbo);

    if (FAILED(hr))
    {
        LOG(Error, "WinDX11Drawcall : VBO object creation failed");
        throw std::exception("VBO object creation failed");
    }

    // Transfer our VBO to the newly created buffer
    D3D11_MAPPED_SUBRESOURCE sub;
    auto ctx = ((WinDX11Renderer*)renderer)->GetImmediateContext();

    hr = ctx->Map(m_vbo, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &sub);
    memcpy(sub.pData, data, dataLength);
    ctx->Unmap(m_vbo, NULL);

    if (FAILED(hr))
    {
        LOG(Error, "WinDX11Drawcall : VBO mapping to CPU failed");
        throw std::exception("VBO mapping to CPU failed");
    }

    m_verticesCount = size;
    */
}