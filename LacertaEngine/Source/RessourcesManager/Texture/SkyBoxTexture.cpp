#include "SkyBoxTexture.h"

#include <DirectXTex.h>

#include "../../Logger/Logger.h"
#include "../../Rendering/GraphicsEngine.h"
#include "../../Rendering/WinDX11/WinDX11Renderer.h"

namespace LacertaEngine
{
    
SkyBoxTexture::SkyBoxTexture()
{
}

SkyBoxTexture::~SkyBoxTexture()
{
    RendererType rdrType = GraphicsEngine::Get()->GetRendererType();

    switch (rdrType)
    {
    case RENDERER_WIN_DX11:
        {
            if(ID3D11Resource* Tex = static_cast<ID3D11Resource*>(m_resource))
                Tex->Release();

            if(ID3D11ShaderResourceView* Srv = static_cast<ID3D11ShaderResourceView*>(m_resourceView))
                Srv->Release();
        }
    }
}

void SkyBoxTexture::CreateResource(const wchar_t* filePath)
{
    RendererType rdrType = GraphicsEngine::Get()->GetRendererType();

    switch (rdrType)
    {
    case RENDERER_WIN_DX11:
        {
            
        }
    }
}

}

