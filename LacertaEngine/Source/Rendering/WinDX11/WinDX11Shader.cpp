#include "WinDX11Shader.h"
#include <codecvt>
#include <Windows.h>
#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"
#include <d3dcompiler.h>
#include "../GraphicsEngine.h"

namespace LacertaEngine
{

D3D11_INPUT_ELEMENT_DESC screenLayout[]=
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

D3D11_INPUT_ELEMENT_DESC meshLayout[]=
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    { "TEXCOORD", 0,  DXGI_FORMAT_R32G32_FLOAT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA ,0 },
    { "NORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,D3D11_INPUT_PER_VERTEX_DATA ,0 }
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

void WinDX11Shader::Load(Renderer* renderer, DrawcallType Type)
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

    switch (Type)
    {
        case dcScreen:
        {
            UINT layoutSize = ARRAYSIZE(screenLayout);

            WinDX11Renderer* localDriver = (WinDX11Renderer*)renderer;
            hr = ((ID3D11Device*)(localDriver->GetDriver()))->CreateInputLayout(
                screenLayout,
                layoutSize,
                m_vertexShaderByteCode,
                m_vertexByteCodeSize,
                &m_vertexLayout);

            m_vertexLayoutStride  = sizeof(VertexDataScreen);

            break;
        }
        case dcMesh:
        {
            UINT layoutSize = ARRAYSIZE(meshLayout);

            WinDX11Renderer* localDriver = (WinDX11Renderer*)renderer;
            hr = ((ID3D11Device*)(localDriver->GetDriver()))->CreateInputLayout(
                meshLayout,
                layoutSize,
                m_vertexShaderByteCode,
                m_vertexByteCodeSize,
                &m_vertexLayout);

            m_vertexLayoutStride  = sizeof(VertexMesh);

            break;
        }
    }

    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed input layout creation !");
        throw std::exception("Create Input Layout failed");
    }

    m_loaded = true;
}

void WinDX11Shader::PreparePass(Renderer* renderer, Drawcall* dc)
{
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    ctx->IASetInputLayout(m_vertexLayout);

    unsigned int offsets = 0;

    ID3D11Buffer* vbo = (ID3D11Buffer*)dc->GetVBO();

    ctx->IASetVertexBuffers(0, 1, &vbo, &m_vertexLayoutStride, &offsets);

    if(dc->GetType() == DrawcallType::dcMesh)
    {
        MeshConstantBuffer meshCb;
        meshCb.LocalMatrix = dc->LocalMatrix();
        GraphicsEngine::Get()->UpdateMeshConstants(&meshCb);
        ctx->IASetIndexBuffer((ID3D11Buffer*)dc->GetIBO(), DXGI_FORMAT_R32_UINT, 0);
    }
    
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(m_vertexShader, nullptr, 0);
    ctx->PSSetShader(m_fragmentShader, nullptr, 0);
}

void WinDX11Shader::Pass(Renderer* renderer, Drawcall* dc)
{
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    unsigned long vertices = dc->GetVerticesCount();

    DrawcallType dcType = dc->GetType();
    switch(dcType)
    {
        case DrawcallType::dcScreen:
        {
            ctx->Draw(vertices, 0);
            break;
        }
        
        case DrawcallType::dcMesh:
        {
            ctx->DrawIndexed(dc->GetIndexListSize(), 0, 0);
            break;  
        }
    }
}

}
