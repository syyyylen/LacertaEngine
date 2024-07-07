#include "WinDX12RenderTarget.h"

#include "WinDX12Renderer.h"
#include "../../Logger/Logger.h"

namespace LacertaEngine
{
    
WinDX12RenderTarget::WinDX12RenderTarget()
{
}

WinDX12RenderTarget::~WinDX12RenderTarget()
{
}

void WinDX12RenderTarget::Initialize(Renderer* renderer, int width, int height, RenderTargetType renderTargetType, int numRt)
{
    LOG(Debug, "WinDX12RenderTarget : Initialize");

    m_renderTargetType = renderTargetType;
    if(numRt > 1)
        m_numRt = numRt;    

    m_width = width;
    m_length = height;

    ReloadBuffers(renderer, width, height);
    SetViewportSize(renderer, width, height);
}

void WinDX12RenderTarget::SetActive(Renderer* renderer)
{
    auto winDX12Renderer = (WinDX12Renderer*)renderer;
    auto view = m_rtvHandles[0].CPU;
    winDX12Renderer->GetCommandList()->OMSetRenderTargets(1, &view, true, nullptr);
}

void WinDX12RenderTarget::SetActive(Renderer* renderer, int idx)
{
    auto winDX12Renderer = (WinDX12Renderer*)renderer;
    auto view = m_rtvHandles[idx].CPU;
    winDX12Renderer->GetCommandList()->OMSetRenderTargets(1, &view, true, nullptr);
}

void WinDX12RenderTarget::ReloadBuffers(Renderer* renderer, unsigned width, unsigned height)
{
    auto winDX12Renderer = (WinDX12Renderer*)renderer;

    switch (m_renderTargetType)
    {
    case BackBuffer:
        {
            winDX12Renderer->GetGraphicsQueue()->FlushCommandQueue();
            HRESULT hr = winDX12Renderer->GetCommandList()->Reset(winDX12Renderer->GetCommandAllocator(), nullptr);
            if(FAILED(hr))
            {
                LOG(Error, "CommandList : failed to Reset");
                std::string errorMsg = std::system_category().message(hr);
                LOG(Error, errorMsg);
            }

            for(int i = 0; i < winDX12Renderer->GetSwapChainBufferCount(); ++i)
            {
                if(winDX12Renderer->m_swapChainBuffer[i])
                    winDX12Renderer->m_swapChainBuffer[i]->Release();

                if(m_rtvHandles[i].IsValid())
                    winDX12Renderer->GetRtvHeap()->Free(m_rtvHandles[i]);
            }

            winDX12Renderer->GetSwapChain()->ResizeBuffers(winDX12Renderer->GetSwapChainBufferCount(), width, height,
                winDX12Renderer->GetSwapChainFormat(), DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

            winDX12Renderer->SetCurrentBackbuffer(0);

            for (UINT i = 0; i < winDX12Renderer->GetSwapChainBufferCount(); i++)
            {
                hr = winDX12Renderer->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&winDX12Renderer->m_swapChainBuffer[i]));
                if(FAILED(hr))
                {
                    LOG(Error, "SwapChain : failed to get buffer");
                    std::string errorMsg = std::system_category().message(hr);
                    LOG(Error, errorMsg);
                }

                m_rtvHandles[i] = winDX12Renderer->GetRtvHeap()->Allocate();
                winDX12Renderer->GetDevice()->CreateRenderTargetView(winDX12Renderer->m_swapChainBuffer[i], nullptr, m_rtvHandles[i].CPU);
            }

            winDX12Renderer->GetCommandList()->Close();
            ID3D12CommandList* cmdLists[] = {  winDX12Renderer->GetCommandList() };
            winDX12Renderer->GetGraphicsQueue()->GetCommandQueue()->ExecuteCommandLists(1, cmdLists);

            winDX12Renderer->GetGraphicsQueue()->FlushCommandQueue();
        }
        break;
    case Texture2D:
        break;
    case TextureCube:
        break;
    default: ;
    }
}

void WinDX12RenderTarget::Resize(Renderer* renderer, unsigned width, unsigned height)
{
    ReloadBuffers(renderer, width, height);
}

void WinDX12RenderTarget::Clear(Renderer* renderer, Vector4 color)
{
    Clear(renderer, color, 0);
}

void WinDX12RenderTarget::Clear(Renderer* renderer, Vector4 color, int idx)
{
    FLOAT clearColor[] = { color.X, color.Y, color.Z, color.W };
    auto winDX12Renderer = (WinDX12Renderer*)renderer;
    auto view = m_rtvHandles[idx].CPU;
    winDX12Renderer->GetCommandList()->ClearRenderTargetView(view, clearColor, 0, nullptr);
}

void WinDX12RenderTarget::SetViewportSize(Renderer* renderer, UINT width, UINT height)
{
}

Texture* WinDX12RenderTarget::CreateTextureFromDepth()
{
    return nullptr;
}

}