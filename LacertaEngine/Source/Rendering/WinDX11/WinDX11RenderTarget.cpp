#include "WinDX11RenderTarget.h"

#include "WinDX11Renderer.h"
#include "WinDX11Texture.h"
#include "../../Logger/Logger.h"

namespace LacertaEngine
{
    
WinDX11RenderTarget::WinDX11RenderTarget()
{
}

WinDX11RenderTarget::~WinDX11RenderTarget()
{
    delete m_renderTargets;
}

void WinDX11RenderTarget::Initialize(Renderer* renderer, int width, int height, RenderTargetType renderTargetType, int numRt)
{
    LOG(Debug, "WinDX11RenderTarget : Initialize");

    m_renderTargetType = renderTargetType;
    if(numRt > 1)
        m_numRt = numRt;    

    m_width = width;
    m_length = height;
    
    ReloadBuffers(renderer, width, height);
    SetViewportSize(renderer, width, height);
}

void WinDX11RenderTarget::SetActive(Renderer* renderer)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    localRenderer->GetImmediateContext()->OMSetRenderTargets(1, &m_renderTargets[0], m_depthStencils[0]);
}

void WinDX11RenderTarget::SetActive(Renderer* renderer, int idx)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    localRenderer->GetImmediateContext()->OMSetRenderTargets(1, &m_renderTargets[idx], m_depthStencils[idx]);
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
            m_renderTargets = new ID3D11RenderTargetView*[1];
                
            hr = localRenderer->GetDXGISwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&buffer);
            if(FAILED(hr))
            {
                LOG(Error, "Failed Backbuffer creation");
                throw std::exception("Failed Backbuffer creation");
            }
    
            hr = device->CreateRenderTargetView(buffer, nullptr, &m_renderTargets[0]);
            if(FAILED(hr))
            {
                LOG(Error, "Failed Render Target creation");
                throw std::exception("Failed Render Target creation");
            }

            break;
        }

        case RenderTargetType::Texture2D:
        {
            m_renderTargets = new ID3D11RenderTargetView*[m_numRt];
            m_targetTexture = new WinDX11Texture();
            int bindFlags = SRV | RTV;
            TextureType type = m_numRt > 1 ? TextureType::Tex2DArray : TextureType::Tex2D;
            buffer = (ID3D11Texture2D*)m_targetTexture->Create(renderer, (int)width, (int)height, type, m_numRt, 1, bindFlags);

            for(int i = 0; i < m_numRt; i++)
            {
                D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
                renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                renderTargetViewDesc.ViewDimension = m_numRt > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DARRAY : D3D11_RTV_DIMENSION_TEXTURE2D; 
                renderTargetViewDesc.Texture2D.MipSlice = 0;
                if(m_numRt > 1)
                {
                    renderTargetViewDesc.Texture2DArray.ArraySize = 1;
                    renderTargetViewDesc.Texture2DArray.MipSlice = 0;
                    renderTargetViewDesc.Texture2DArray.FirstArraySlice = i;
                }

                hr = device->CreateRenderTargetView(buffer, &renderTargetViewDesc, &m_renderTargets[i]);
                if(FAILED(hr))
                {
                    std::string errorMsg = std::system_category().message(hr);
                    LOG(Error, errorMsg);
                    LOG(Error, "Failed Render Target creation");
                    throw std::exception("Failed Render Target creation");
                }
            }

            break;
        }

        case RenderTargetType::TextureCube:
        {
            m_numRt = 6;
            m_targetTexture = new WinDX11Texture();
            int bindFlags = SRV | RTV | UAV;
            buffer = (ID3D11Texture2D*)m_targetTexture->Create(renderer, (int)width, (int)height, TextureType::TexCube, m_numRt, 1, bindFlags);

            // TODO create render targets
            
            break;
        }
    }

    buffer->Release();

    m_depthStencils = new ID3D11DepthStencilView*[m_numRt];
    
    // Depth buffer 
    D3D11_TEXTURE2D_DESC tex_desc = {};
    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    tex_desc.Usage = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    tex_desc.MipLevels = 1;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.MiscFlags = 0;
    tex_desc.ArraySize = m_numRt;
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

    D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc;
    ZeroMemory(&depthSrvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
    depthSrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS ;
    depthSrvDesc.ViewDimension = m_numRt > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DARRAY : D3D11_SRV_DIMENSION_TEXTURE2D;
    if(m_numRt > 1)
    {
        depthSrvDesc.Texture2DArray.ArraySize = m_numRt;
        depthSrvDesc.Texture2DArray.MipLevels = 1;
        depthSrvDesc.Texture2DArray.FirstArraySlice = 0;
        depthSrvDesc.Texture2DArray.MostDetailedMip = 0;
    }
    depthSrvDesc.Texture2D.MipLevels = 1;
    depthSrvDesc.Texture2D.MostDetailedMip = 0;
    
    hr = device->CreateTexture2D(&tex_desc, nullptr, &buffer);
    if(FAILED(hr))
    {
        LOG(Error, "Failed depth buffer creation");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
        throw std::exception("Failed depth buffer creation");
    }
    
    hr = device->CreateShaderResourceView(buffer, &depthSrvDesc, &m_depthShaderResView);
    if(FAILED(hr))
    {
        LOG(Error, "Failed depth buffer srv creation");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
        throw std::exception("Failed depth buffer srv creation");
    }

    for(int i = 0; i < m_numRt; i++)
    {
        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        dsvDesc.Flags = 0;
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.Texture2D.MipSlice = 0;
        dsvDesc.ViewDimension = m_numRt > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DARRAY : D3D11_DSV_DIMENSION_TEXTURE2D;
        if(m_numRt > 1)
        {
            dsvDesc.Texture2DArray.ArraySize = 1;
            dsvDesc.Texture2DArray.MipSlice = 0;
            dsvDesc.Texture2DArray.FirstArraySlice = i;
        }

        hr = device->CreateDepthStencilView(buffer, &dsvDesc, &m_depthStencils[i]);
        if(FAILED(hr))
        {
            LOG(Error, "Failed depth buffer creation");
            std::string errorMsg = std::system_category().message(hr);
            LOG(Error, errorMsg);
            throw std::exception("Failed depth buffer creation");
        }
    }

    hr = device->CreateDepthStencilState(&desc, &m_depthStencilState);
    if(FAILED(hr))
    {
        LOG(Error, "Failed depth state buffer creation");
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
        throw std::exception("Failed depth state buffer creation");
    }
    localRenderer->GetImmediateContext()->OMSetDepthStencilState(m_depthStencilState, 1);
    
    buffer->Release();
}

void WinDX11RenderTarget::Resize(Renderer* renderer, unsigned width, unsigned height)
{
    for(int i = 0; i < m_numRt; i++)
    {
        m_renderTargets[i]->Release();
        m_depthStencils[i]->Release();
    }
    
    delete m_targetTexture;

    if(m_depthShaderResView)
        m_depthShaderResView->Release();

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
    Clear(renderer, color, 0);
}

void WinDX11RenderTarget::Clear(Renderer* renderer, Vector4 color, int idx)
{
    ID3D11DeviceContext* ctx = ((WinDX11Renderer*)renderer)->GetImmediateContext();
    FLOAT clearColor[] = { color.X, color.Y, color.Z, color.W };
    ctx->ClearRenderTargetView(m_renderTargets[idx], clearColor);
    ctx->ClearDepthStencilView(m_depthStencils[idx], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void WinDX11RenderTarget::SetViewportSize(Renderer* renderer, UINT width, UINT height)
{
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    D3D11_VIEWPORT vp = {};
    vp.Width = (float)width;
    vp.Height = (float)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    localRenderer->GetImmediateContext()->RSSetViewports(1, &vp);
}

Texture* WinDX11RenderTarget::CreateTextureFromDepth()
{
    auto tex = new WinDX11Texture();
    tex->SetSRV(m_depthShaderResView);
    tex->SetNumTexs(m_numRt);
    return tex;
}
    
}
