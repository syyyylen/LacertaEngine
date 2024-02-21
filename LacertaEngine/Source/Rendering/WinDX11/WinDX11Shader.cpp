#include "WinDX11Shader.h"
#include <codecvt>
#include <Windows.h>
#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"
#include <d3dcompiler.h>
#include "../RHI.h"
#include "../Material.h"

namespace LacertaEngine
{
    
D3D11_INPUT_ELEMENT_DESC sceneMeshLayout[]=
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    { "TEXCOORD", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA ,0 },
    { "NORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,D3D11_INPUT_PER_VERTEX_DATA ,0 },
    { "TANGENT", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,D3D11_INPUT_PER_VERTEX_DATA ,0 },
    { "BINORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 44,D3D11_INPUT_PER_VERTEX_DATA ,0 }
};
    
WinDX11Shader::WinDX11Shader()
{
}

WinDX11Shader::~WinDX11Shader()
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

void WinDX11Shader::Load(Renderer* renderer, DrawableLayout layout)
{
    if(m_loaded)
        return;

    ID3D11Device* localDev = (ID3D11Device*)renderer->GetDriver();
    HRESULT hr = localDev->CreateVertexShader(m_vertexShaderByteCode, m_vertexByteCodeSize, nullptr, &m_vertexShader);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed vertex shader creation !");
    }

    hr = localDev->CreatePixelShader(m_pixelShaderByteCode, m_pixelByteCodeSize, nullptr, &m_fragmentShader);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed pixel shader creation !");
    }

    switch (layout)
    {
    case DrawableLayout::SceneMesh:
        UINT layoutSize = ARRAYSIZE(sceneMeshLayout);

        WinDX11Renderer* localDriver = (WinDX11Renderer*)renderer;
        hr = ((ID3D11Device*)(localDriver->GetDriver()))->CreateInputLayout(
            sceneMeshLayout,
            layoutSize,
            m_vertexShaderByteCode,
            m_vertexByteCodeSize,
            &m_vertexLayout);

        m_vertexLayoutStride  = sizeof(SceneVertexMesh);
        break;
    }

    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed input layout creation !");
        throw std::exception("Create Input Layout failed");
    }

    m_loaded = true;
}

void WinDX11Shader::PreparePass(Renderer* renderer)
{
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    ctx->IASetInputLayout(m_vertexLayout);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(m_vertexShader, nullptr, 0);
    ctx->PSSetShader(m_fragmentShader, nullptr, 0);

    ID3D11SamplerState* SamplerState = driver->GetSamplerState();
    ctx->PSSetSamplers(0, 1, &SamplerState);
}

void WinDX11Shader::Pass(Renderer* renderer, Drawcall* dc)
{
    RHI::Get()->SetRasterizerState(dc->GetShaderName() == "SkyboxShader"); // TODO remove this plz
    
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();
    auto drawable = dc->GetDrawable();

    ctx->DrawIndexed(drawable->GetIndicesSize(), 0, 0);
}

}
