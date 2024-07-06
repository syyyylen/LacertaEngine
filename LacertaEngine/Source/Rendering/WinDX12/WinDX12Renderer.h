#pragma once
#include "../Renderer.h"
#include "../../Core.h"
#include <d3d12.h>
#include <dxgi1_6.h>

#include "WinDX12CommandQueue.h"
#include "WinDX12DescriptorHeap.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API WinDX12Renderer : public Renderer
{
public:
    WinDX12Renderer();
    ~WinDX12Renderer();
    
    void Initialize(int* context, int width, int height, int targetRefreshRate) override;
    void LoadShaders() override;
    void SetBackbufferRenderTargetActive() override;
    void PresentSwapChain() override;
    void OnResizeWindow(unsigned width, unsigned height) override;
    void UpdateConstantBuffer(void* buffer, ConstantBufferType cbufType) override;
    void SetRasterizerCullState(bool cullFront) override;
    void SetSamplerState(bool comparisonSampler) override;
    RenderTarget* CreateRenderTarget(int width, int height, RenderTargetType renderTargetType, int& outRTidx, int numRt) override;
    void ExecuteComputeShader(std::string name, UINT x, UINT y, UINT z) override;
    Mesh* ImportMesh(std::string filePath) override;
    Texture* CreateTexture(const wchar_t* filePath, int idx) override;
    Texture* CreateTexture(int width, int height, TextureType type, int num, int mipNum, int bindFlags) override;
    int* GetDriver() override { return nullptr; }

    void WaitForAllQueuesIdle();

    ID3D12Device* GetDevice() { return m_device; }
    IDXGIAdapter1* GetAdapter() { return m_adapter; }
    IDXGIFactory4* GetFactory() { return m_factory; }

    WinDX12CommandQueue* GetGraphicsQueue() { return m_graphicsQueue; }
    WinDX12CommandQueue* GetComputeQueue() { return m_computeQueue; }
    WinDX12CommandQueue* GetCopyQueue() { return m_copyQueue; }

    UINT FramesInFlight() { return 1; } // TODO fix this
    DXGI_FORMAT GetSwapChainFormat() { return m_swapChainFormat; }

    WinDX12DescriptorHeap* GetRtvHeap() { return m_rtvHeap; }
    WinDX12DescriptorHeap* GetSrvHeap() { return m_srvHeap; }

    ID3D12GraphicsCommandList* GetCommandList() { return m_commandList; }

    // TODO remove temp debug D3D12 functions
    DescriptorHandle GetBackBufferSrvHandle() { return m_backbufferSrvHandles[m_currentBackbuffer]; }
    void RenderToRT();
    void Present();

private:
    ID3D12Device* m_device;
    ID3D12Debug* m_debug;
    ID3D12DebugDevice* m_debugDevice;
    IDXGIFactory4* m_factory;
    IDXGIAdapter1* m_adapter;

    static const UINT m_swapChainBufferCount = 2;
    DXGI_FORMAT m_swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    ID3D12Resource* m_swapChainBuffer[m_swapChainBufferCount];
    IDXGISwapChain* m_swapChain;
    int m_currentBackbuffer = 0;

    WinDX12CommandQueue* m_graphicsQueue;
    WinDX12CommandQueue* m_computeQueue;
    WinDX12CommandQueue* m_copyQueue;

    ID3D12CommandAllocator* m_commandAllocator;
    ID3D12GraphicsCommandList* m_commandList;

    WinDX12DescriptorHeap* m_rtvHeap;
    WinDX12DescriptorHeap* m_srvHeap;
    DescriptorHandle m_backBufferRtvHandles[2];
    DescriptorHandle m_backbufferSrvHandles[2];

    bool m_msaaEnabled = false;
    UINT m_msaaQualityLevel;

    // TODO remove them
    int cachedWidth;
    int cachedHeight;
};
    
}
