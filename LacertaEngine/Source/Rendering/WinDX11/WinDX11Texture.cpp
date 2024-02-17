#include "WinDX11Texture.h"

#include <DirectXTex.h>
#include "../../Logger/Logger.h"
#include "../../Rendering/WinDX11/WinDX11Renderer.h"

namespace LacertaEngine
{
    
WinDX11Texture::WinDX11Texture()
{
}

WinDX11Texture::~WinDX11Texture()
{
}

void WinDX11Texture::CreateResource(const wchar_t* filePath, Renderer* renderer)
{
    WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)renderer;
    ID3D11Device* Device = (ID3D11Device*)Dx11Renderer->GetDriver();

    bool isDDSFile = false;

    const wchar_t* suffix = L"dds";
    size_t pathLength = wcslen(filePath);
    size_t suffixLength = wcslen(suffix);
    if(suffixLength < pathLength)
        isDDSFile = wcsncmp(filePath + pathLength - suffixLength, suffix, suffixLength) == 0;

    if(isDDSFile)
    {
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

        m_resource = Tex;
        m_resourceView = Srv;

        return;
    }

    DirectX::ScratchImage imageData;
    HRESULT res = DirectX::LoadFromWICFile(filePath, DirectX::WIC_FLAGS_NONE, nullptr, imageData);

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
        desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
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

    m_resource = Tex;
    m_resourceView = Srv;
}

void* WinDX11Texture::GetResource()
{
    return m_resource;
}

void* WinDX11Texture::GetResourceView()
{
    return m_resourceView;
}

}

