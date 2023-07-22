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
    void Clear(Renderer* renderer, Color color) override;
    void SetViewportSize(Renderer* renderer, UINT width, UINT height) override;

private:
    ID3D11RenderTargetView* m_renderTarget;
    D3D11_VIEWPORT* m_viewport;
};

}
