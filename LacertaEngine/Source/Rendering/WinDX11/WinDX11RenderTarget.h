#pragma once
#include <d3d11.h>
#include "../RenderTarget.h"
#include "../../Core.h"

namespace LacertaEngine
{

class WinDX11RenderTarget : public RenderTarget
{
public:
    WinDX11RenderTarget();
    virtual ~WinDX11RenderTarget();

    void Initialize(Renderer* renderer, int width, int height, RenderTargetType renderTargetType, int numRt) override;
    void SetActive(Renderer* renderer) override;
    void SetActive(Renderer* renderer, int idx) override;
    void ReloadBuffers(Renderer* renderer, unsigned width, unsigned height);
    void Resize(Renderer* renderer, unsigned width, unsigned height) override;
    void Clear(Renderer* renderer, Vector4 color) override;
    void Clear(Renderer* renderer, Vector4 color, int idx) override;
    void SetViewportSize(Renderer* renderer, UINT width, UINT height) override;
    void* GetSRV() override;
    void* GetDepthSRV() override;
    Texture* CreateTextureFromRT(int texBindIdx) override;
    Texture* CreateTextureFromDepth(int texBindIdx) override;

    ID3D11ShaderResourceView* GetTextureShaderResView() const { return m_targetTextureShaderResView; }

private:
    ID3D11ShaderResourceView* m_targetTextureShaderResView;
    ID3D11UnorderedAccessView* m_targetTextureUAV;
    ID3D11ShaderResourceView* m_depthShaderResView;
    ID3D11RenderTargetView** m_renderTargets;
    ID3D11DepthStencilView** m_depthStencils;
    ID3D11DepthStencilState* m_depthStencilState;
};

}
