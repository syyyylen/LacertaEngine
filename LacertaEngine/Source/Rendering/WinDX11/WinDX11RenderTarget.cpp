#include "WinDX11RenderTarget.h"

#include "WinDX11Renderer.h"
#include "../../Logger/Logger.h"

LacertaEngine::WinDX11RenderTarget::WinDX11RenderTarget()
{
}

LacertaEngine::WinDX11RenderTarget::~WinDX11RenderTarget()
{
    delete m_viewport;
    delete m_renderTarget;
}

void LacertaEngine::WinDX11RenderTarget::Initialize(Renderer* renderer, int width, int height, int depth)
{
    LOG(Debug, "WinDX11RenderTarget : Initialize");
    
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

    m_viewport = new D3D11_VIEWPORT();
    SetViewportSize(renderer, width, height);
}

void LacertaEngine::WinDX11RenderTarget::SetActive(Renderer* renderer)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    localRenderer->GetImmediateContext()->OMSetRenderTargets(1, &m_renderTarget, nullptr);
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
    ZeroMemory(m_viewport, sizeof(D3D11_VIEWPORT));
    m_viewport->Width = (float)width;
    m_viewport->Height = (float)height;
    m_viewport->MinDepth = 0.0f;
    m_viewport->MaxDepth = 1.0f;
    localRenderer->GetImmediateContext()->RSSetViewports(1, m_viewport);
}
