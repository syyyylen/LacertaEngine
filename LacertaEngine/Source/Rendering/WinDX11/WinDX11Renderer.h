#pragma once
#include "../Renderer.h"
#include "../../Core.h"
#include <d3d11.h>
#include <dxgi1_3.h>

namespace LacertaEngine
{

struct WinDX11Cbuf
{
    ID3D11Buffer* Buffer;
    UINT Slot;
};

class LACERTAENGINE_API WinDX11Renderer : public Renderer
{
public:
    WinDX11Renderer();
    ~WinDX11Renderer();

    void Initialize(int* context, int width, int height, int targetRefreshRate) override;
    void CreateRenderTarget(int width, int height) override;
    void LoadShaders() override;
    
    void SetBackbufferRenderTargetActive() override;
    void PresentSwapChain() override;
    void OnResizeWindow(unsigned width, unsigned height) override;
    void UpdateConstantBuffer(void* buffer, ConstantBufferType cbufType) override;
    void SetRasterizerCullState(bool cullFront) override;

    ID3D11Buffer* CreateVBO(std::vector<VertexMesh> vertices);
    ID3D11Buffer* CreateIBO(std::vector<unsigned> indices);
    
    Mesh* CreateMesh(const wchar_t* filePath) override;
    Texture* CreateTexture(const wchar_t* filePath, int idx) override;
    
    int* GetDriver() override { return (int*)m_device; }

    IDXGISwapChain* GetDXGISwapChain() { return m_dxgiSwapChain; }
    ID3D11DeviceContext* GetImmediateContext() { return m_deviceContext; }
    ID3D11SamplerState* GetSamplerState() const { return m_samplerState; }

private:
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_deviceContext;
    ID3D11RasterizerState* m_rasterizerCullBackState;
    ID3D11RasterizerState* m_rasterizerCullFrontState;
    IDXGISwapChain* m_dxgiSwapChain;
    ID3D11SamplerState* m_samplerState;
    D3D_FEATURE_LEVEL m_featureLevel;
    ID3D11Buffer* m_constantBuffer; 

    Vector2 m_previousViewportSize;

    std::map<ConstantBufferType, WinDX11Cbuf> m_constantBuffers;
};

}

