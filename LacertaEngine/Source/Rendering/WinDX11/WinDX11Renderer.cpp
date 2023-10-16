#include "WinDX11Renderer.h"

#include <d3dcompiler.h>

#include "WinDX11Drawcall.h"
#include "WinDX11RenderTarget.h"
#include "WinDX11Shader.h"
#include "../Drawcall.h"
#include "../../Logger/Logger.h"
#include "../../RessourcesManager/Mesh/Mesh.h"

namespace LacertaEngine
{
    
WinDX11Renderer::WinDX11Renderer()
{
}

WinDX11Renderer::~WinDX11Renderer()
{
    if(m_constantBuffer)
        m_constantBuffer->Release();

    if(m_meshConstantBuffer)
        m_meshConstantBuffer->Release();
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
    desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
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

    ConstantBuffer cb;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(ConstantBuffer);
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &cb;

    if(FAILED(m_device->CreateBuffer(&bufferDesc, &initData, &m_constantBuffer)))
    {
        LOG(Error, "Create Constant Buffer failed");
        throw std::exception("Create Constant Buffer failed");
    }

    MeshConstantBuffer meshCb;

    D3D11_BUFFER_DESC meshBufferDesc = {};
    meshBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    meshBufferDesc.ByteWidth = sizeof(MeshConstantBuffer);
    meshBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    meshBufferDesc.CPUAccessFlags = 0;
    meshBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA meshInitData = {};
    meshInitData.pSysMem = &meshCb;

    if(FAILED(m_device->CreateBuffer(&meshBufferDesc, &meshInitData, &m_meshConstantBuffer)))
    {
        LOG(Error, "Create Mesh Constant Buffer failed");
        throw std::exception("Create Mesh Constant Buffer failed");
    }

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

    ID3D11RasterizerState* rasterizerState;
    m_device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    m_deviceContext->RSSetState(rasterizerState);

    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
}

void WinDX11Renderer::CreateRenderTarget(int width, int height, int depth)
{
    LOG(Debug, "WinDX11Renderer : Create Render Target");

    WinDX11RenderTarget* newRendTarg = new WinDX11RenderTarget();
    newRendTarg->Initialize(this, width, height, depth);
    m_renderTargets.emplace_back(newRendTarg);

    WinDX11RenderTarget* textureRendTarg = new WinDX11RenderTarget();
    textureRendTarg->SetRenderToTexture(true);
    textureRendTarg->Initialize(this, width, height, depth);
    m_renderTargets.emplace_back(textureRendTarg);
}

void WinDX11Renderer::LoadShaders()
{
    //TODO load procedurally all the shaders instead of hard coding this compilation

    LOG(Debug, "Loading Shaders");
    
    WinDX11Shader* MeshShader = new WinDX11Shader();

    // Compiling & Creating Vertex Shader
    ID3DBlob* vertexErrorBlob = nullptr;
    ID3DBlob* vertexBlob;
    
    HRESULT hr = D3DCompileFromFile(L"../LacertaEngine/Source/Rendering/Shaders/MeshVertex.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", 0, 0, &vertexBlob, &vertexErrorBlob);
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

    MeshShader->SetVSData(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize());

    // Compiling & Creating Pixel Shader
    ID3DBlob* pixelErrorBlob = nullptr;
    ID3DBlob* pixelBlob;
    
    hr = D3DCompileFromFile(L"../LacertaEngine/Source/Rendering/Shaders/MeshPixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", 0, 0, &pixelBlob, &pixelErrorBlob);
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

    MeshShader->SetPSData(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize());

    m_shaders.emplace("MeshShader", MeshShader);
}

void WinDX11Renderer::AddDrawcall(DrawcallData* dcData)
{
    WinDX11Drawcall* dc = new WinDX11Drawcall();
    dc->Setup(this, dcData);
    
    m_drawcalls.push_back(dc);
}

void WinDX11Renderer::CreateBuffers(ShapeData& shapeData, std::vector<VertexMesh> vertices, std::vector<unsigned> indices)
{
    unsigned long dataLength = vertices.size() * (unsigned long)sizeof(VertexMesh);

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

    D3D11_BUFFER_DESC iboDesc = {};
    iboDesc.Usage = D3D11_USAGE_DEFAULT;
    iboDesc.ByteWidth = 4 * indices.size();
    iboDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    iboDesc.CPUAccessFlags = 0;
    iboDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iboInitData;
    iboInitData.pSysMem = &indices[0];

    ID3D11Buffer* ibo;
    hr = m_device->CreateBuffer(&iboDesc, &iboInitData, &ibo);

    if (FAILED(hr))
    {
        LOG(Error, "WinDX11Drawcall : IBO object creation failed");
        throw std::exception("IBO object creation failed");
    }

    shapeData.Vbo = vbo;
    shapeData.Ibo = ibo;
}

void WinDX11Renderer::RenderFrame(Vector2 ViewportSize)
{
    // We scale the scene render target accordingly to the editor viewport size
    m_renderTargets[1]->SetViewportSize(this, (UINT)ViewportSize.X, (UINT)ViewportSize.Y);
    if(m_previousViewportSize.X != ViewportSize.X || m_previousViewportSize.Y != m_previousViewportSize.Y)
    {
        m_renderTargets[1]->Resize(this, ViewportSize.X, ViewportSize.Y);
        m_previousViewportSize = ViewportSize;
    }

    m_renderTargets[1]->SetActive(this);
    m_renderTargets[1]->Clear(this, Vector4(0.0f, 0.0f, 0.0f, 0.0f));

    for(auto dc : m_drawcalls)
        dc->Pass(this);

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

void WinDX11Renderer::UpdateConstantBuffer(void* buffer)
{
    m_deviceContext->UpdateSubresource(m_constantBuffer, NULL, NULL, buffer, NULL, NULL);
    m_deviceContext->VSSetConstantBuffers(0, 1, &m_constantBuffer);
    m_deviceContext->PSSetConstantBuffers(0, 1, &m_constantBuffer);
}

void WinDX11Renderer::UpdateMeshConstantBuffer(void* buffer)
{
    m_deviceContext->UpdateSubresource(m_meshConstantBuffer, NULL, NULL, buffer, NULL, NULL);
    m_deviceContext->VSSetConstantBuffers(1, 1, &m_meshConstantBuffer);
    m_deviceContext->PSSetConstantBuffers(1, 1, &m_meshConstantBuffer);
}
    
}
