#include "DX11Texture.h"
#include "../../Logger/Logger.h"
#include "../../Rendering/GraphicsEngine.h"
#include "../../Rendering/WinDX11/WinDX11Renderer.h"
#include <DirectXTex.h>

namespace LacertaEngine
{

DX11Texture::DX11Texture()
{
}

DX11Texture::~DX11Texture()
{
    m_texture->Release();
    m_shaderResView->Release();
}

void DX11Texture::CreateResource(const wchar_t* filePath)
{
    WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)GraphicsEngine::Get()->GetRenderer();
    ID3D11Device* Device = (ID3D11Device*)Dx11Renderer->GetDriver();
    
    DirectX::ScratchImage imageData;
    HRESULT res = DirectX::LoadFromWICFile(filePath, DirectX::WIC_FLAGS_NONE, nullptr, imageData);

    if(SUCCEEDED(res))
    {
        res = DirectX::CreateTexture(Device, imageData.GetImages(), imageData.GetImageCount(), imageData.GetMetadata(), &m_texture);

        if(!SUCCEEDED(res))
        {
            std::string errorMsg = std::system_category().message(res);
            LOG(Error, errorMsg);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = imageData.GetMetadata().format;
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        desc.Texture2D.MipLevels = imageData.GetMetadata().mipLevels;
        desc.Texture2D.MostDetailedMip = 0;
        res = Device->CreateShaderResourceView(m_texture, &desc, &m_shaderResView);

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
}
    
}
