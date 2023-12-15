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
            WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)GraphicsEngine::Get()->GetRenderer();
            ID3D11Device* Device = (ID3D11Device*)Dx11Renderer->GetDriver();
            
            DirectX::ScratchImage imageData;
            DirectX::DDS_FLAGS flags = DirectX::DDS_FLAGS::DDS_FLAGS_NONE;
            
            HRESULT res = DirectX::LoadFromDDSFile(filePath, flags, nullptr, imageData);

            ID3D11Resource* Tex = nullptr;
            ID3D11ShaderResourceView* Srv = nullptr;
            if(SUCCEEDED(res))
            {
                res = DirectX::CreateTexture(Device, imageData.GetImages(), imageData.GetImageCount(), imageData.GetMetadata(), &Tex);

                if(!SUCCEEDED(res))
                {
                    std::string errorMsg = std::system_category().message(res);
                    LOG(Error, errorMsg);
                }

                D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
                desc.Format = imageData.GetMetadata().format;
                desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                desc.Texture2D.MipLevels = imageData.GetMetadata().mipLevels;
                desc.Texture2D.MostDetailedMip = 0;

                res = Device->CreateShaderResourceView(Tex, &desc, &Srv);

                if(!SUCCEEDED(res))
                {
                    std::string errorMsg = std::system_category().message(res);
                    LOG(Error, errorMsg);
                    throw std::exception("Shader res view creation failed");
                }
            }
            else
            {
                std::string errorMsg = std::system_category().message(res);
                LOG(Error, errorMsg);
                throw std::exception("Texture not created");
            }

            m_resource = (void*)Tex;
            m_resourceView = (void*)Srv;
        }
    }
}

}

