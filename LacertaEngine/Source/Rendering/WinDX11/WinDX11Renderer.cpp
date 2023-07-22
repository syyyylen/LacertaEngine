#include "WinDX11Renderer.h"

#include "../../Logger/Logger.h"

LacertaEngine::WinDX11Renderer::WinDX11Renderer()
{
}

LacertaEngine::WinDX11Renderer::~WinDX11Renderer()
{
}

void LacertaEngine::WinDX11Renderer::Initialize(int* context, int width, int height, int targetRefreshRate)
{
    LOG(Debug, "WinDX11Renderer : Initialize");
}

void LacertaEngine::WinDX11Renderer::CreateRenderTarget(int width, int height, int depth)
{
    LOG(Debug, "WinDX11Renderer : Create Render Target");
}

void LacertaEngine::WinDX11Renderer::RenderFrame()
{
}
