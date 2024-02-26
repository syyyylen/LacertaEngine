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

void WinDX11RenderTarget::Initialize(Renderer* renderer, int width, int height, RenderTargetType renderTargetType)
{
    LOG(Debug, "WinDX11RenderTarget : Initialize");

    m_renderTargetType = renderTargetType;
    
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
    HRESULT hr;

    switch (m_renderTargetType)
    {
    case RenderTargetType::BackBuffer:
        {
            hr = localRenderer->GetDXGISwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
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

            break;
        }

    case RenderTargetType::Texture2D:
        {
            D3D11_TEXTURE2D_DESC textureDesc = {};
            textureDesc.Width = width;
            textureDesc.Height = height;
            textureDesc.MipLevels = 1;
            textureDesc.ArraySize = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.Usage = D3D11_USAGE_DEFAULT;
            textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
            textureDesc.CPUAccessFlags = 0;
            textureDesc.MiscFlags = 0;
            
            hr = device->CreateTexture2D(&textureDesc, NULL, &buffer);
            if(FAILED(hr))
            {
                std::string errorMsg = std::system_category().message(hr);
                LOG(Error, errorMsg);
                LOG(Error, "Failed scene texture creation");
                throw std::exception("Failed scene texture creation");
            }

            D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
            renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
            renderTargetViewDesc.Texture2D.MipSlice = 0;
            
            hr = device->CreateRenderTargetView(buffer, &renderTargetViewDesc, &m_renderTarget);
            if(FAILED(hr))
            {
                std::string errorMsg = std::system_category().message(hr);
                LOG(Error, errorMsg);
                LOG(Error, "Failed Render Target creation");
                throw std::exception("Failed Render Target creation");
            }

            D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
            shaderResourceViewDesc.Format = textureDesc.Format;
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.MipLevels = 1;
            hr = device->CreateShaderResourceView(buffer, &shaderResourceViewDesc, &m_targetTextureShaderResView);
            if(FAILED(hr))
            {
                std::string errorMsg = std::system_category().message(hr);
                LOG(Error, errorMsg);
                LOG(Error, "Failed Scene texture Shader Res View creation");
                throw std::exception("Failed Scene texture Shader Res View creation");
            }

            break;
        }

    case RenderTargetType::TextureCube:
        {
            // TODO implement

            break;
        }
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

    D3D11_DEPTH_STENCIL_DESC desc = {};
    desc.DepthEnable = true;
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

    desc.StencilEnable = true;
    desc.StencilReadMask = 0xFF;
    desc.StencilWriteMask = 0xFF;

    desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    
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

    hr = device->CreateDepthStencilState(&desc, &m_depthStencilState);
    if(FAILED(hr))
    {
        LOG(Error, "Failed depth state buffer creation");
        throw std::exception("Failed depth state buffer creation");
    }
    localRenderer->GetImmediateContext()->OMSetDepthStencilState(m_depthStencilState, 1);
    
    buffer->Release();
}

void WinDX11RenderTarget::Resize(Renderer* renderer, unsigned width, unsigned height)
{
    if(m_renderTarget)
        m_renderTarget->Release();

    if(m_targetTextureShaderResView)
        m_targetTextureShaderResView->Release();

    if(m_depthStencil)
        m_depthStencil->Release();

    // If this render target is the backbuffer, we want to resize it on app window size change
    if(m_renderTargetType == RenderTargetType::BackBuffer)
    {
        WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
        // carefull with the buffer count (curr : 2, set to 0 to preserve all)
        // see : https://learn.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgiswapchain-resizebuffers
        localRenderer->GetDXGISwapChain()->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    }

    // We need to resize the textures to match the resolution
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

void* WinDX11RenderTarget::GetSRV()
{
    return (void*)GetTextureShaderResView();
}
    
}
