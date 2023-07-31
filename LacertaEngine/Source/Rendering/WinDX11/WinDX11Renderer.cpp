#include "WinDX11Renderer.h"

#include "WinDX11Drawcall.h"
#include "WinDX11RenderTarget.h"
#include "../Drawcall.h"
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
    rasterizerDesc.CullMode = D3D11_CULL_NONE; // D3D11_CULL_NONE = Disable culling of face with counterclockwise vertices indexes. Curr activated // TODO D3D11_CULL_BACK
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
}

void WinDX11Renderer::CreateRenderTarget(int width, int height, int depth)
{
    LOG(Debug, "WinDX11Renderer : Create Render Target");

    m_renderTarget = new WinDX11RenderTarget();
    m_renderTarget->Initialize(this, width, height, depth);
}

void WinDX11Renderer::AddDrawcall(DrawcallData* dcData)
{
    LOG(Debug, "WinDX11Renderer : Adding a drawcall");

    WinDX11Drawcall* dc = new WinDX11Drawcall();
    
    dc->Setup(this, dcData);

    dc->CreateVBO(this, dcData->Data, dcData->Size);

    if(dcData->Type == DrawcallType::Mesh)
        dc->CreateIBO(this, dcData->IndexesData, dcData->IndexesSize);
    
    m_drawcalls.push_back(dc);
}

void WinDX11Renderer::RenderFrame()
{
    m_renderTarget->SetActive(this);
    m_renderTarget->Clear(this, Vector4(0.0f, 0.0f, 0.0f, 1.0f));

    for(auto dc : m_drawcalls)
        dc->Pass(this);
}

void WinDX11Renderer::PresentSwapChain()
{
    m_dxgiSwapChain->Present(true, NULL);   
}

void WinDX11Renderer::OnResize(unsigned width, unsigned height)
{
    WinDX11RenderTarget* rendTarg = (WinDX11RenderTarget*)m_renderTarget;
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
