#include "WinDX11Shader.h"
#include <codecvt>
#include <Windows.h>
#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"
#include <d3dcompiler.h>
#include "../GraphicsEngine.h"
#include "../Material.h"

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

    if(dc->GetType() == DrawcallType::dcMesh) // TODO clean all this
    {
        MeshConstantBuffer meshCb;

        if(dc->GetMaterial()->IsSkyBox())
        {
            meshCb.LocalMatrix = dc->LocalMatrix();
            meshCb.HasAlbedo = false;
            meshCb.HasNormalMap = false;

            const auto skyboxTex = dc->GetMaterial()->GetTexture(0);
            if(skyboxTex != nullptr)
            {
                const auto baseColorSrv = static_cast<ID3D11ShaderResourceView*>(skyboxTex->m_resourceView);
                if(baseColorSrv != nullptr)
                {
                    meshCb.HasAlbedo = true;
                    ctx->VSSetShaderResources(2, 1, &baseColorSrv);
                    ctx->PSSetShaderResources(2, 1, &baseColorSrv);
                }
            }

            const auto irradianceTex = dc->GetMaterial()->GetTexture(1);
            if(irradianceTex != nullptr)
            {
                const auto irrSrv = static_cast<ID3D11ShaderResourceView*>(irradianceTex->m_resourceView);
                if(irrSrv != nullptr)
                {
                    ctx->VSSetShaderResources(3, 1, &irrSrv);
                    ctx->PSSetShaderResources(3, 1, &irrSrv);
                }
            }

            const auto BRDFLut = dc->GetMaterial()->GetTexture(2);
            if(BRDFLut != nullptr)
            {
                const auto BRDFsrv = static_cast<ID3D11ShaderResourceView*>(BRDFLut->m_resourceView);
                if(BRDFsrv != nullptr)
                {
                    ctx->VSSetShaderResources(7, 1, &BRDFsrv);
                    ctx->PSSetShaderResources(7, 1, &BRDFsrv);
                }
            }

            GraphicsEngine::Get()->SetRasterizerState(true);
        }
        else
        {
            meshCb.LocalMatrix = dc->LocalMatrix();
            meshCb.LightProperties = dc->GetMaterial()->GetMatLightProperties();

            const auto baseColor = dc->GetMaterial()->GetTexture(0);
            if(baseColor != nullptr)
            {
                const auto baseColorSrv = static_cast<ID3D11ShaderResourceView*>(baseColor->m_resourceView);
                if(baseColorSrv != nullptr)
                {
                    meshCb.HasAlbedo = true;
                    ctx->VSSetShaderResources(0, 1, &baseColorSrv);
                    ctx->PSSetShaderResources(0, 1, &baseColorSrv);
                }
            }
            else
            {
                meshCb.HasAlbedo = false;
            }

            const auto normalMap = dc->GetMaterial()->GetTexture(1);
            if(normalMap != nullptr)
            {
                const auto normalMapSrv = static_cast<ID3D11ShaderResourceView*>(normalMap->m_resourceView);
                if(normalMapSrv != nullptr)
                {
                    meshCb.HasNormalMap = true;
                    ctx->VSSetShaderResources(1, 1, &normalMapSrv);
                    ctx->PSSetShaderResources(1, 1, &normalMapSrv);
                }
            }
            else
            {
                meshCb.HasNormalMap = false;
            }

            const auto roughness = dc->GetMaterial()->GetTexture(2);
            if(roughness != nullptr)
            {
                const auto roughnessSrv = static_cast<ID3D11ShaderResourceView*>(roughness->m_resourceView);
                if(roughnessSrv != nullptr)
                {
                    meshCb.HasRoughness = true;
                    ctx->VSSetShaderResources(4, 1, &roughnessSrv);
                    ctx->PSSetShaderResources(4, 1, &roughnessSrv);
                }
            }
            else
            {
                meshCb.HasRoughness = false;
            }

            const auto metallic = dc->GetMaterial()->GetTexture(3);
            if(metallic != nullptr)
            {
                const auto metallicSrv = static_cast<ID3D11ShaderResourceView*>(metallic->m_resourceView);
                if(metallicSrv != nullptr)
                {
                    meshCb.HasMetallic = true;
                    ctx->VSSetShaderResources(5, 1, &metallicSrv);
                    ctx->PSSetShaderResources(5, 1, &metallicSrv);
                }
            }
            else
            {
                meshCb.HasMetallic = false;
            }

            const auto ao = dc->GetMaterial()->GetTexture(4);
            if(ao != nullptr)
            {
                const auto aoSrv = static_cast<ID3D11ShaderResourceView*>(ao->m_resourceView);
                if(aoSrv != nullptr)
                {
                    meshCb.HasAmbiant = true;
                    ctx->VSSetShaderResources(6, 1, &aoSrv);
                    ctx->PSSetShaderResources(6, 1, &aoSrv);
                }
            }
            else
            {
                meshCb.HasAmbiant = false;
            }

            GraphicsEngine::Get()->SetRasterizerState(false);
        }
            
        GraphicsEngine::Get()->UpdateMeshConstants(&meshCb);
        ctx->IASetIndexBuffer((ID3D11Buffer*)dc->GetIBO(), DXGI_FORMAT_R32_UINT, 0);
    }
    
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(m_vertexShader, nullptr, 0);
    ctx->PSSetShader(m_fragmentShader, nullptr, 0);

    ID3D11SamplerState* SamplerState = driver->GetSamplerState();
    ctx->PSSetSamplers(0, 1, &SamplerState);
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
