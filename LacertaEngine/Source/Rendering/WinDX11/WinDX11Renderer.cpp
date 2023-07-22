#include "WinDX11Renderer.h"

#include "WinDX11RenderTarget.h"
#include "../Drawcall.h"
#include "../../Logger/Logger.h"
    
LacertaEngine::WinDX11Renderer::WinDX11Renderer()
{
}

LacertaEngine::WinDX11Renderer::~WinDX11Renderer()
{
}

void LacertaEngine::WinDX11Renderer::Initialize(int* context, int width, int height, int targetRefreshRate)
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
    desc.Windowed = TRUE;
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
    desc.Flags = 0;
    desc.OutputWindow = wnd;

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
}

void LacertaEngine::WinDX11Renderer::CreateRenderTarget(int width, int height, int depth)
{
    LOG(Debug, "WinDX11Renderer : Create Render Target");

    m_renderTarget = new WinDX11RenderTarget();
    m_renderTarget->Initialize(this, width, height, depth);
}

void LacertaEngine::WinDX11Renderer::RenderFrame()
{
    if(m_renderTarget)
    {
        m_renderTarget->SetActive(this);
        m_renderTarget->Clear(this, Color(255.0f, 240.0f, 0.0f, 1.0f));
    }

    if(m_dxgiSwapChain)
    {
        m_dxgiSwapChain->Present(true, NULL);   
    }
}

