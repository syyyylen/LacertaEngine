#include "WinDX11Shader.h"
#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"

D3D11_INPUT_ELEMENT_DESC CBuffer_VertexDesc_Full[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

D3D11_INPUT_ELEMENT_DESC CBuffer_VertexDesc_Screen[] =
{
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

LacertaEngine::WinDX11Shader::WinDX11Shader()
{
}

LacertaEngine::WinDX11Shader::~WinDX11Shader()
{
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        delete m_vertexShader;
    }

    if (m_fragmentShader)
    {
        m_fragmentShader->Release();
        delete m_fragmentShader;
    }
}

void LacertaEngine::WinDX11Shader::Load(Renderer* renderer)
{
    LOG(Debug, "WinDX11Shader : Load");
    
    // TODO load a pixel and vertex shader

    // TODO get the device & device->CreateVertexShader 
    // TODO get the device & device->CreatePixelShader
    
    // TODO get the device & device->CreateInputLayout 
}

void LacertaEngine::WinDX11Shader::PreparePass(Renderer* renderer, Drawcall* dc)
{
    LOG(Debug, "WinDX11Shader : PreparePass");
    
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    ctx->IASetInputLayout(m_vertexLayout);

    unsigned int offsets = 0;

    ID3D11Buffer* vbo = (ID3D11Buffer*)dc->GetVBO();
    ctx->IASetVertexBuffers(0, 1, &vbo, &m_vertexLayoutStride, &offsets);

    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(m_vertexShader, 0, 0);
    ctx->PSSetShader(m_fragmentShader, 0, 0);
}

void LacertaEngine::WinDX11Shader::Pass(Renderer* renderer, Drawcall* dc)
{
    LOG(Debug, "WinDX11Shader : Pass");
    
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    unsigned long vertices = dc->GetVerticesCount();
    ctx->Draw(vertices, 0);
}
