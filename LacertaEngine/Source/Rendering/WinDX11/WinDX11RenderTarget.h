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

    void Initialize(Renderer* renderer, int width, int height) override;
    void SetActive(Renderer* renderer) override;
    void ReloadBuffers(Renderer* renderer, unsigned width, unsigned height);
    void Resize(Renderer* renderer, unsigned width, unsigned height) override;
    void Clear(Renderer* renderer, Vector4 color) override;
    void SetViewportSize(Renderer* renderer, UINT width, UINT height) override;

    void SetRenderToTexture(bool renderToTexture) { m_renderToTexture = renderToTexture; }
    bool RenderToTexture() const { return m_renderToTexture; }

    ID3D11ShaderResourceView* GetTextureShaderResView() const { return m_targetTextureShaderResView; }
    ID3D11RenderTargetView* GetRtv() { return m_renderTarget; }

private:
    bool m_renderToTexture;
    ID3D11ShaderResourceView* m_targetTextureShaderResView;
    ID3D11RenderTargetView* m_renderTarget;
    ID3D11DepthStencilView* m_depthStencil;
    ID3D11DepthStencilState* m_depthStencilState;
};

}
