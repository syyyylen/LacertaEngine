#pragma once
#include "../Renderer.h"
#include "../../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API WinDX11Renderer : public Renderer
{
public:
    WinDX11Renderer();
    ~WinDX11Renderer();

    void Initialize(int* context, int width, int height, int targetRefreshRate) override;
    void CreateRenderTarget(int width, int height, int depth) override;

    void RenderFrame() override;
};

}

