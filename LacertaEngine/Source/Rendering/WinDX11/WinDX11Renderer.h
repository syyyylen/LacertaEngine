﻿#pragma once
#include "../Renderer.h"
#include "../../Core.h"
#include "../RenderTarget.h"

#include <d3d11.h>
#include <dxgi1_3.h>
#include <map>

namespace LacertaEngine
{

class LACERTAENGINE_API WinDX11Renderer : public Renderer
{
public:
    WinDX11Renderer();
    ~WinDX11Renderer();

    void Initialize(int* context, int width, int height, int targetRefreshRate) override;
    void CreateRenderTarget(int width, int height, int depth) override;
    void LoadShaders() override;
    
    void RenderFrame(Vector2 ViewportSize) override;
    void PresentSwapChain() override;
    void OnResizeWindow(unsigned width, unsigned height) override;
    void UpdateConstantBuffer(void* buffer) override;
    void UpdateMeshConstantBuffer(void* buffer) override;
    void SetRasterizerCullState(bool cullFront) override;
    void AddDrawcall(DrawcallData* dcData) override;
    void CreateBuffers(ShapeData& shapeData, std::vector<VertexMesh> vertices, std::vector<unsigned> indices) override;

    ID3D11Buffer* CreateVBO(std::vector<VertexMesh> vertices);
    ID3D11Buffer* CreateIBO(std::vector<unsigned> indices);
    
    Mesh* CreateMesh(const wchar_t* filePath) override;
    Texture* CreateTexture(const wchar_t* filePath) override;
    
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
    ID3D11Buffer* m_meshConstantBuffer;

    Vector2 m_previousViewportSize;
};

}

