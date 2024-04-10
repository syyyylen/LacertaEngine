#include "WinDX11Renderer.h"

#include <d3dcompiler.h>

#include "WinDX11Mesh.h"
#include "WinDX11Texture.h"
#include "WinDX11RenderTarget.h"
#include "WinDX11Shader.h"
#include "../Drawcall.h"
#include "../SkyBoxPassLayouts.h"
#include "../ShadowMapPassLayouts.h"
#include "../../Logger/Logger.h"

namespace LacertaEngine
{
    
WinDX11Renderer::WinDX11Renderer()
{
}

WinDX11Renderer::~WinDX11Renderer()
{
    if(m_constantBuffer)
        m_constantBuffer->Release();
    
    for(auto cb : m_constantBuffers)
        cb.second.Buffer->Release();

    m_constantBuffers.clear();
}

void WinDX11Renderer::Initialize(int* context, int width, int height, int targetRefreshRate)
{
    LOG(Debug, "WinDX11Renderer : Initialize");

    HWND wnd = (HWND)context;

    D3D_FEATURE_LEVEL featureLevels[]=
    {
        D3D_FEATURE_LEVEL_11_0
    };

    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    DXGI_SWAP_CHAIN_DESC desc;
    ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
    desc.BufferCount = 2;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.RefreshRate.Numerator = targetRefreshRate;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SampleDesc.Count = 1;      
    desc.SampleDesc.Quality = 0; 
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    desc.OutputWindow = wnd;
    desc.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,                   
        D3D_DRIVER_TYPE_HARDWARE,
        0,                          
        deviceFlags,               
        featureLevels,                     
        ARRAYSIZE(featureLevels),          
        D3D11_SDK_VERSION,          
        &desc,
        &m_dxgiSwapChain,
        &m_device,                  // Returns the Direct3D device created.
        &m_featureLevel,          // Returns feature level of device created.
        &m_deviceContext            // Returns the device immediate context.
    );

    if(FAILED(hr))
    {
        LOG(Error, "Failed Device & SwapChain creation");
        throw std::exception("Failed SwapChain creation");
    }

    m_constantBuffers.emplace(ConstantBufferType::SceneCbuf, CreateConstantBuffer(0, sizeof(SceneConstantBuffer)));
    m_constantBuffers.emplace(ConstantBufferType::MeshCbuf, CreateConstantBuffer(1, sizeof(SceneMeshConstantBuffer)));
    m_constantBuffers.emplace(ConstantBufferType::SkyBoxCbuf, CreateConstantBuffer(2, sizeof(SkyBoxConstantBuffer)));
    m_constantBuffers.emplace(ConstantBufferType::SMLightCbuf, CreateConstantBuffer(3, sizeof(ShadowMapLightConstantBuffer)));
    m_constantBuffers.emplace(ConstantBufferType::PrefilterCbuf, CreateConstantBuffer(4, sizeof(PrefilterMapConstantBuffer)));

    // Changing rasterizer properties & state 
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK; // D3D11_CULL_NONE = Disable culling of face with counterclockwise vertices indexes
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = true;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.MultisampleEnable = false;
    rasterizerDesc.AntialiasedLineEnable = false;

    m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerCullBackState);

    // We create a cullback state for cases when we need to render inside a mesh (skybox)
    D3D11_RASTERIZER_DESC cullfrontRasterizerDesc;
    ZeroMemory(&cullfrontRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    cullfrontRasterizerDesc.FillMode = D3D11_FILL_SOLID;
    cullfrontRasterizerDesc.CullMode = D3D11_CULL_FRONT;
    cullfrontRasterizerDesc.FrontCounterClockwise = false;
    cullfrontRasterizerDesc.DepthBias = 0;
    cullfrontRasterizerDesc.SlopeScaledDepthBias = 0.0f;
    cullfrontRasterizerDesc.DepthBiasClamp = 0.0f;
    cullfrontRasterizerDesc.DepthClipEnable = true;
    cullfrontRasterizerDesc.ScissorEnable = false;
    cullfrontRasterizerDesc.MultisampleEnable = false;
    cullfrontRasterizerDesc.AntialiasedLineEnable = false;

    m_device->CreateRasterizerState(&cullfrontRasterizerDesc, &m_rasterizerCullFrontState);

    // Set default state to cull back
    m_deviceContext->RSSetState(m_rasterizerCullBackState);

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    if(FAILED(m_device->CreateSamplerState(&samplerDesc, &m_samplerState)))
    {
        LOG(Error, "Create Sampler State failed");
        throw std::exception("Create Sampler State failed");
    }

    D3D11_SAMPLER_DESC comparisonSamplerDesc;
    ZeroMemory(&comparisonSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
    comparisonSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    comparisonSamplerDesc.BorderColor[0] = 1.0f;
    comparisonSamplerDesc.BorderColor[1] = 1.0f;
    comparisonSamplerDesc.BorderColor[2] = 1.0f;
    comparisonSamplerDesc.BorderColor[3] = 1.0f;
    comparisonSamplerDesc.MinLOD = 0.f;
    comparisonSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    comparisonSamplerDesc.MipLODBias = 0.f;
    comparisonSamplerDesc.MaxAnisotropy = 0;
    comparisonSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    comparisonSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;

    if(FAILED(m_device->CreateSamplerState(&comparisonSamplerDesc, &m_comparisonSamplerState)))
    {
        LOG(Error, "Create Sampler State failed");
        throw std::exception("Create Sampler State failed");
    }
}

void WinDX11Renderer::LoadShaders()
{
    LOG(Debug, "Loading Shaders");

    m_shaders.clear();

    m_shaders.emplace("MeshPBRShader", CompileShader(L"../LacertaEngine/Source/Rendering/Shaders/MeshVertex.hlsl", L"../LacertaEngine/Source/Rendering/Shaders/MeshPBRPixelShader.hlsl"));
    m_shaders.emplace("SkyboxShader", CompileShader(L"../LacertaEngine/Source/Rendering/Shaders/SkyBoxVertexShader.hlsl", L"../LacertaEngine/Source/Rendering/Shaders/SkyBoxPixelShader.hlsl"));
    m_shaders.emplace("ShadowMapShader", CompileShader(L"../LacertaEngine/Source/Rendering/Shaders/ShadowMapShader.hlsl", L"../LacertaEngine/Source/Rendering/Shaders/ShadowMapPixelShader.hlsl"));

    m_computeShaders.clear();
    m_computeShaders.emplace("IrradianceCS", CompileComputeShader(L"../LacertaEngine/Source/Rendering/Shaders/IrradianceComputeShader.hlsl"));
    m_computeShaders.emplace("PrefilterCS", CompileComputeShader(L"../LacertaEngine/Source/Rendering/Shaders/PreFilterEnvMapComputeShader.hlsl"));
}

ID3D11Buffer* WinDX11Renderer::CreateVBO(std::vector<SceneVertexMesh> vertices)
{
    unsigned long dataLength = vertices.size() * (unsigned long)sizeof(SceneVertexMesh);

    D3D11_BUFFER_DESC vboDesc = {};
    vboDesc.Usage = D3D11_USAGE_DEFAULT;
    vboDesc.ByteWidth = dataLength;
    vboDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vboDesc.CPUAccessFlags = 0;
    vboDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vboInitData;
    vboInitData.pSysMem = &vertices[0];

    ID3D11Buffer* vbo;
    HRESULT hr = m_device->CreateBuffer(&vboDesc, &vboInitData, &vbo);

    if (FAILED(hr))
    {
        LOG(Error, "WinDX11Drawcall : VBO object creation failed");
        throw std::exception("VBO object creation failed");
    }

    return vbo;
}

ID3D11Buffer* WinDX11Renderer::CreateIBO(std::vector<unsigned> indices)
{
    D3D11_BUFFER_DESC iboDesc = {};
    iboDesc.Usage = D3D11_USAGE_DEFAULT;
    iboDesc.ByteWidth = 4 * indices.size();
    iboDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    iboDesc.CPUAccessFlags = 0;
    iboDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iboInitData;
    iboInitData.pSysMem = &indices[0];

    ID3D11Buffer* ibo;
    HRESULT hr = m_device->CreateBuffer(&iboDesc, &iboInitData, &ibo);

    if (FAILED(hr))
    {
        LOG(Error, "WinDX11Drawcall : IBO object creation failed");
        throw std::exception("IBO object creation failed");
    }

    return ibo;
}

Mesh* WinDX11Renderer::CreateMesh(const wchar_t* filePath)
{
    for(auto resource : m_graphicsResources)
    {
        if(filePath == resource->GetFilePath())
            return dynamic_cast<Mesh*>(resource);
    }
    
    Mesh* mesh = new WinDX11Mesh();
    mesh->CreateResource(filePath, this);
    m_graphicsResources.push_back(mesh);
    
    return mesh;
}

Texture* WinDX11Renderer::CreateTexture(const wchar_t* filePath, int idx)
{
    for(auto resource : m_graphicsResources)
    {
        if(filePath == resource->GetFilePath())
            return dynamic_cast<Texture*>(resource);
    }
    
    Texture* tex = new WinDX11Texture();
    tex->CreateResource(filePath, this);
    tex->SetTextureIdx(idx);
    m_graphicsResources.push_back(tex);

    return tex;
}

Texture* WinDX11Renderer::CreateTexture(int width, int height, TextureType type, int num, int mipNum, int bindFlags)
{
    Texture* tex = new WinDX11Texture();
    tex->Create(this, width, height, type, num, mipNum, bindFlags);
    return tex;
}

WinDX11Shader* WinDX11Renderer::CompileShader(LPCWSTR VSFilePath, LPCWSTR PSFilePath)
{
    auto Shader = new WinDX11Shader();

    ID3DBlob* vertexErrorBlob = nullptr;
    ID3DBlob* vertexBlob;

    HRESULT hr = D3DCompileFromFile(VSFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &vertexBlob, &vertexErrorBlob);
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

    Shader->SetVSData(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize());

    ID3DBlob* pixelErrorBlob = nullptr;
    ID3DBlob* pixelBlob;

    hr = D3DCompileFromFile(PSFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &pixelBlob, &pixelErrorBlob);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed pixel shader compilation !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);

        if (pixelErrorBlob) 
        {
            std::string errorMessage(static_cast<const char*>(pixelErrorBlob->GetBufferPointer()), pixelErrorBlob->GetBufferSize());
            LOG(Error, errorMessage);
            pixelErrorBlob->Release();
        }
    }

    Shader->SetPSData(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize());

    return Shader;
}

ID3D11ComputeShader* WinDX11Renderer::CompileComputeShader(LPCWSTR CSFilePath)
{
    ID3DBlob* csErrorBlob = nullptr;
    ID3DBlob* csBlob;

    HRESULT hr = D3DCompileFromFile(CSFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "cs_5_0", 0, 0, &csBlob, &csErrorBlob);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed compute shader compilation !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);

        if (csErrorBlob) 
        {
            std::string errorMessage(static_cast<const char*>(csErrorBlob->GetBufferPointer()), csErrorBlob->GetBufferSize());
            LOG(Error, errorMessage);
            csErrorBlob->Release();
        }
    }

    ID3D11ComputeShader* cs;
    hr = m_device->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &cs);
    if(FAILED(hr))
    {
        LOG(Error, "WinDX11Shader : Failed compute shader creation !");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
    }
    
    return cs;
}

void WinDX11Renderer::SetBackbufferRenderTargetActive()
{
    // setting back the backbuffer render target
    m_renderTargets[0]->SetActive(this); 
}

void WinDX11Renderer::PresentSwapChain()
{
    m_dxgiSwapChain->Present(true, NULL);   
}

void WinDX11Renderer::OnResizeWindow(unsigned width, unsigned height)
{
    WinDX11RenderTarget* rendTarg = (WinDX11RenderTarget*)m_renderTargets[0];
    if(rendTarg == nullptr)
        return;
    
    rendTarg->Resize(this, width, height);
}

void WinDX11Renderer::UpdateConstantBuffer(void* buffer, ConstantBufferType cbufType)
{
    auto winDX11cbuf = m_constantBuffers.find(cbufType)->second;
    m_deviceContext->UpdateSubresource(winDX11cbuf.Buffer, NULL, NULL, buffer, NULL, NULL);
    m_deviceContext->VSSetConstantBuffers(winDX11cbuf.Slot, 1, &winDX11cbuf.Buffer);
    m_deviceContext->PSSetConstantBuffers(winDX11cbuf.Slot, 1, &winDX11cbuf.Buffer);
    m_deviceContext->CSSetConstantBuffers(winDX11cbuf.Slot, 1, &winDX11cbuf.Buffer);
}

void WinDX11Renderer::SetRasterizerCullState(bool cullFront)
{
    cullFront ? m_deviceContext->RSSetState(m_rasterizerCullFrontState) :  m_deviceContext->RSSetState(m_rasterizerCullBackState); 
}

void WinDX11Renderer::SetSamplerState(bool comparisonSampler)
{
    ID3D11SamplerState* SamplerState = comparisonSampler ? m_comparisonSamplerState : m_samplerState;
    m_deviceContext->PSSetSamplers(0, 1, &SamplerState);
    m_deviceContext->PSSetSamplers(1, 1, &m_comparisonSamplerState);
}

RenderTarget* WinDX11Renderer::CreateRenderTarget(int width, int height, RenderTargetType renderTargetType, int& outRTidx, int numRt)
{
    WinDX11RenderTarget* textureRendTarg = new WinDX11RenderTarget();
    textureRendTarg->Initialize(this, width, height, renderTargetType, numRt);
    m_renderTargets.emplace_back(textureRendTarg);

    outRTidx = (int)m_renderTargets.size() - 1;
    return textureRendTarg;
}

void WinDX11Renderer::ExecuteComputeShader(std::string name, UINT x, UINT y, UINT z)
{
    auto CS = m_computeShaders.find(name)->second;
    m_deviceContext->CSSetSamplers(0, 1, &m_samplerState);
    m_deviceContext->CSSetShader(CS, nullptr, 0);
    m_deviceContext->Dispatch(x, y, z);
}

WinDX11Cbuf WinDX11Renderer::CreateConstantBuffer(UINT slot, UINT size)
{
    ID3D11Buffer* b0;
    
    D3D11_BUFFER_DESC bufferDesc{};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    
    if(FAILED(m_device->CreateBuffer(&bufferDesc, NULL, &b0)))
    {
        LOG(Error, "Create Constant Buffer failed");
        throw std::exception("Create Constant Buffer failed");
    }

    WinDX11Cbuf cbuf;
    cbuf.Buffer = b0;
    cbuf.Slot = slot;

    return cbuf;
}
    
}
