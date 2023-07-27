#include "WinDX11Shader.h"

#include <codecvt>

#include <Windows.h>

#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"

#include <d3dcompiler.h>

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

void LacertaEngine::WinDX11Shader::Load(Renderer* renderer, const wchar_t* vertexShaderName, const wchar_t* pixelShaderName)
{
    LOG(Debug, "WinDX11Shader : Load");

    // Compiling & Creating Vertex Shader
    ID3DBlob* vertexErrorBlob = nullptr;
    ID3DBlob* vertexBlob;
    
    HRESULT hr = D3DCompileFromFile(vertexShaderName, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexBlob, &vertexErrorBlob);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed vertex shader compilation !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);

        if (vertexErrorBlob) 
        {
            std::string errorMessage(static_cast<const char*>(vertexErrorBlob->GetBufferPointer()), vertexErrorBlob->GetBufferSize());
            LOG(Error, errorMessage);
            vertexErrorBlob->Release();
        }
    }

    void* vertexShaderByteCode = vertexBlob->GetBufferPointer();
    size_t vertexByteCodeSize = vertexBlob->GetBufferSize();

    ID3D11Device* localDev = (ID3D11Device*)renderer->GetDriver();
    hr = localDev->CreateVertexShader(vertexShaderByteCode, vertexByteCodeSize, nullptr, &m_vertexShader);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed vertex shader creation !");
    }

    // Compiling & Creating Pixel Shader
    ID3DBlob* pixelErrorBlob = nullptr;
    ID3DBlob* pixelBlob;
    
    hr = D3DCompileFromFile(pixelShaderName, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelBlob, &pixelErrorBlob);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed pixel shader compilation !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);

        if (vertexErrorBlob) 
        {
            std::string errorMessage(static_cast<const char*>(pixelErrorBlob->GetBufferPointer()), pixelErrorBlob->GetBufferSize());
            LOG(Error, errorMessage);
            pixelErrorBlob->Release();
        }
    }

    void* pixelShaderByteCode = pixelBlob->GetBufferPointer();
    size_t pixelByteCodeSize = pixelBlob->GetBufferSize();

    hr = localDev->CreatePixelShader(pixelShaderByteCode, pixelByteCodeSize, nullptr, &m_fragmentShader);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed pixel shader creation !");
    }

    D3D11_INPUT_ELEMENT_DESC layout[]=
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    UINT layoutSize = ARRAYSIZE(layout);

    WinDX11Renderer* localDriver = (WinDX11Renderer*)renderer;
    hr = ((ID3D11Device*)(localDriver->GetDriver()))->CreateInputLayout(
        layout,
        layoutSize,
        vertexShaderByteCode,
        vertexByteCodeSize,
        &m_vertexLayout);

    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed input layout creation !");
        throw std::exception("Create Input Layout failed");
    }

    m_vertexLayoutStride = 12; // TODO concentre toi sale fdp de merde, tu branles quoi ????? (C'est sizeof(la struct passée dans layout))
}

void LacertaEngine::WinDX11Shader::PreparePass(Renderer* renderer, Drawcall* dc)
{
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    ctx->IASetInputLayout(m_vertexLayout);

    unsigned int offsets = 0;

    ID3D11Buffer* vbo = (ID3D11Buffer*)dc->GetVBO();

    ctx->IASetVertexBuffers(0, 1, &vbo, &m_vertexLayoutStride, &offsets);

    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->VSSetShader(m_vertexShader, nullptr, 0);
    ctx->PSSetShader(m_fragmentShader, nullptr, 0);
}

void LacertaEngine::WinDX11Shader::Pass(Renderer* renderer, Drawcall* dc)
{
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    unsigned long vertices = dc->GetVerticesCount();
    ctx->Draw(vertices, 0);
}
