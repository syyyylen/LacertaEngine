#include "WinDX11RenderTarget.h"

#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"

namespace LacertaEngine
{
    
WinDX11RenderTarget::WinDX11RenderTarget()
{
}

WinDX11RenderTarget::~WinDX11RenderTarget()
{
    delete m_renderTarget;
}

void WinDX11RenderTarget::Initialize(Renderer* renderer, int width, int height, int depth)
{
    LOG(Debug, "WinDX11RenderTarget : Initialize");
    
    ReloadBuffers(renderer, width, height);
    SetViewportSize(renderer, width, height);
}

void WinDX11RenderTarget::SetActive(Renderer* renderer)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    localRenderer->GetImmediateContext()->OMSetRenderTargets(1, &m_renderTarget, m_depthStencil);
}

void WinDX11RenderTarget::ReloadBuffers(Renderer* renderer, unsigned width, unsigned height)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    ID3D11Device* device = (ID3D11Device*)renderer->GetDriver();
    ID3D11Texture2D* buffer = NULL;

    HRESULT hr = localRenderer->GetDXGISwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
    if(FAILED(hr))
    {
        LOG(Error, "Failed Backbuffer creation");
        throw std::exception("Failed Backbuffer creation");
    }

    hr = device->CreateRenderTargetView(buffer, nullptr, &m_renderTarget);
    if(FAILED(hr))
    {
        LOG(Error, "Failed Render Target creation");
        throw std::exception("Failed Render Target creation");
    }

    buffer->Release();

    // Depth buffer 
    D3D11_TEXTURE2D_DESC tex_desc = {};
    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    tex_desc.Usage = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    tex_desc.MipLevels = 1;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.MiscFlags = 0;
    tex_desc.ArraySize = 1;
    tex_desc.CPUAccessFlags = 0;

    hr = device->CreateTexture2D(&tex_desc, nullptr, &buffer);
    if(FAILED(hr))
    {
        LOG(Error, "Failed depth buffer creation");
        throw std::exception("Failed depth buffer creation");
    }

    hr = device->CreateDepthStencilView(buffer, NULL, &m_depthStencil);
    if(FAILED(hr))
    {
        LOG(Error, "Failed depth buffer creation");
        throw std::exception("Failed depth buffer creation");
    }

    buffer->Release();
}

void WinDX11RenderTarget::Resize(Renderer* renderer, unsigned width, unsigned height)
{
    if(m_renderTarget)
        m_renderTarget->Release();

    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    // carefull with the buffer count (curr : 2, set to 0 to preserve all)
    // see : https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
    localRenderer->GetDXGISwapChain()->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    ReloadBuffers(renderer, width, height);
}

void WinDX11RenderTarget::Clear(Renderer* renderer, Vector4 color)
{
    ID3D11DeviceContext* ctx = ((WinDX11Renderer*)renderer)->GetImmediateContext();
    FLOAT clearColor[] = { color.X, color.Y, color.Z, color.W };
    ctx->ClearRenderTargetView(m_renderTarget, clearColor);
    ctx->ClearDepthStencilView(m_depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void WinDX11RenderTarget::SetViewportSize(Renderer* renderer, UINT width, UINT height)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    D3D11_VIEWPORT vp = {};
    vp.Width = width;
    vp.Height = height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    localRenderer->GetImmediateContext()->RSSetViewports(1, &vp);
}
    
}
