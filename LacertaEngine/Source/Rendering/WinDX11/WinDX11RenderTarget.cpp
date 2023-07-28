#include "WinDX11RenderTarget.h"

#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"

LacertaEngine::WinDX11RenderTarget::WinDX11RenderTarget()
{
}

LacertaEngine::WinDX11RenderTarget::~WinDX11RenderTarget()
{
    delete m_renderTarget;
}

void LacertaEngine::WinDX11RenderTarget::Initialize(Renderer* renderer, int width, int height, int depth)
{
    LOG(Debug, "WinDX11RenderTarget : Initialize");
    
    ReloadBuffers(renderer, width, height);
    SetViewportSize(renderer, width, height);
}

void LacertaEngine::WinDX11RenderTarget::SetActive(Renderer* renderer)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    localRenderer->GetImmediateContext()->OMSetRenderTargets(1, &m_renderTarget, nullptr);
}

void LacertaEngine::WinDX11RenderTarget::ReloadBuffers(Renderer* renderer, unsigned width, unsigned height)
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
}

void LacertaEngine::WinDX11RenderTarget::Resize(Renderer* renderer, unsigned width, unsigned height)
{
    if(m_renderTarget)
        m_renderTarget->Release();

    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    localRenderer->GetDXGISwapChain()->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    ReloadBuffers(renderer, width, height);
}

void LacertaEngine::WinDX11RenderTarget::Clear(Renderer* renderer, Color color)
{
    ID3D11DeviceContext* ctx = ((WinDX11Renderer*)renderer)->GetImmediateContext();
    FLOAT clearColor[] = { color.R, color.G, color.B, color.A };
    ctx->ClearRenderTargetView(m_renderTarget, clearColor);
}

void LacertaEngine::WinDX11RenderTarget::SetViewportSize(Renderer* renderer, UINT width, UINT height)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    D3D11_VIEWPORT vp = {};
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    localRenderer->GetImmediateContext()->RSSetViewports(1, &vp);
}
