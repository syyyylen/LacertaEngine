#pragma once
#include "../Renderer.h"
#include "../../Core.h"
#include <d3d12.h>
#include <dxgi1_6.h>

namespace LacertaEngine
{
    
class WinDX12Renderer : public Renderer
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

    ID3D12Device* GetDevice() { return m_device; }
    IDXGIAdapter1* GetAdapter() { return m_adapter; }
    IDXGIFactory3* GetFactory() { return m_factory; }

private:
    ID3D12Device* m_device;
    ID3D12Debug* m_debug;
    ID3D12DebugDevice* m_debugDevice;
    IDXGIFactory3* m_factory;
    IDXGIAdapter1* m_adapter;

    bool m_msaaEnabled = false;
    UINT m_msaaQualityLevel;
};
    
}
