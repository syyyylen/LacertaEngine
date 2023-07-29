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

    void Initialize(Renderer* renderer, int width, int height, int depth) override;
    void SetActive(Renderer* renderer) override;
    void ReloadBuffers(Renderer* renderer, unsigned width, unsigned height);
    void Resize(Renderer* renderer, unsigned width, unsigned height);
    void Clear(Renderer* renderer, Vector4 color) override;
    void SetViewportSize(Renderer* renderer, UINT width, UINT height) override;

    ID3D11RenderTargetView* GetRtv() { return m_renderTarget; }

private:
    ID3D11RenderTargetView* m_renderTarget;
};

}
