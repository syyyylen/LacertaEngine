#pragma once
#include "../../Core.h"
#include "../RenderTarget.h"
#include <d3d12.h>

#include "WinDX12DescriptorHeap.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API WinDX12RenderTarget : public RenderTarget
{
public:
    WinDX12RenderTarget();
    virtual ~WinDX12RenderTarget();

    void Initialize(Renderer* renderer, int width, int height, RenderTargetType renderTargetType, int numRt) override;
    void SetActive(Renderer* renderer) override;
    void SetActive(Renderer* renderer, int idx) override;
    void ReloadBuffers(Renderer* renderer, unsigned width, unsigned height);
    void Resize(Renderer* renderer, unsigned width, unsigned height) override;
    void Clear(Renderer* renderer, Vector4 color) override;
    void Clear(Renderer* renderer, Vector4 color, int idx) override;
    void SetViewportSize(Renderer* renderer, UINT width, UINT height) override;
    Texture* CreateTextureFromDepth() override;

private:
    DescriptorHandle m_rtvHandles[2]; // TODO resize dynamically
};
    
}
