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
    m_resourceView->Release();
    // TODO release UAV
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
            desc.Texture2DArray.MipLevels = imageData.GetMetadata().mipLevels;
            desc.Texture2DArray.MostDetailedMip = 0;
            desc.Texture2DArray.FirstArraySlice = 0;
            desc.Texture2DArray.ArraySize = 6;

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

    m_resourceView = Srv;
}

void* WinDX11Texture::Create(Renderer* renderer, int width, int height, TextureType type, int num, int mipNum, int bindFlags)
{
    m_numTex = type == TextureType::TexCube ? 6 : num;
    m_mipMapsLevels = mipNum;
    
    WinDX11Renderer* localRenderer = (WinDX11Renderer*)renderer;
    ID3D11Device* device = (ID3D11Device*)renderer->GetDriver();
    ID3D11Texture2D* buffer = NULL;
    HRESULT hr;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = m_mipMapsLevels;
    textureDesc.ArraySize = type == TextureType::TexCube ? 6 : m_numTex;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    UINT bind;
    if(HasTextureFlag(bindFlags, RTV))
        bind = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    if(HasTextureFlag(bindFlags, UAV))
        bind = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    if(HasTextureFlag(bindFlags, RTV) && HasTextureFlag(bindFlags, UAV))
        bind = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    textureDesc.BindFlags = bind;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = type == TextureType::TexCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

    hr = device->CreateTexture2D(&textureDesc, NULL, &buffer);
    if(FAILED(hr))
    {
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
        LOG(Error, "Failed scene texture creation");
        throw std::exception("Failed scene texture creation");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = textureDesc.Format;
    shaderResourceViewDesc.ViewDimension = m_numTex > 1 ? (type == TextureType::TexCube ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2DARRAY) : D3D11_SRV_DIMENSION_TEXTURE2D;
    if(m_numTex > 1)
    {
        if(type == TextureType::TexCube)
        {
            shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
            shaderResourceViewDesc.TextureCube.MostDetailedMip = 0;
            shaderResourceViewDesc.TextureCube.MipLevels = m_mipMapsLevels;
        }
        else
        {
            shaderResourceViewDesc.Texture2DArray.ArraySize = m_numTex;
            shaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2DArray.MipLevels = m_mipMapsLevels;
            shaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
        }
    }
    else
    {
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = m_mipMapsLevels;
    }
                
    hr = device->CreateShaderResourceView(buffer, &shaderResourceViewDesc, &m_resourceView);
    if(FAILED(hr))
    {
        std::string errorMsg = std::system_category().message(hr);
        LOG(Error, errorMsg);
        LOG(Error, "Failed Scene texture Shader Res View creation");
        throw std::exception("Failed Scene texture Shader Res View creation");
    }

    if(HasTextureFlag(bindFlags, UAV))
    {
        m_unorderedAccessViews = new ID3D11UnorderedAccessView*[m_mipMapsLevels];

        for(int i = 0; i < m_mipMapsLevels; i++)
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
            uavDesc.Format = textureDesc.Format;
            uavDesc.ViewDimension = m_numTex > 1 ?  D3D11_UAV_DIMENSION_TEXTURE2DARRAY : D3D11_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2DArray.MipSlice = i;
            uavDesc.Texture2DArray.ArraySize = m_numTex;
            uavDesc.Texture2DArray.FirstArraySlice = 0;

            hr = device->CreateUnorderedAccessView(buffer, &uavDesc, &m_unorderedAccessViews[i]);
            if(FAILED(hr))
            {
                std::string errorMsg = std::system_category().message(hr);
                LOG(Error, errorMsg);
                LOG(Error, "Failed texture UAV creation");
                throw std::exception("Failed texture UAV creation");
            }
        }
    }
    
    return (void*)buffer;
}

void WinDX11Texture::OnReadWriteAccessChanged(Renderer* renderer)
{
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();
    
    if(m_allowReadWrite)
    {
        ID3D11ShaderResourceView* const srv[1] = { NULL };
        ctx->VSSetShaderResources(m_bindIdx, 1, srv);
        ctx->PSSetShaderResources(m_bindIdx, 1, srv);
        ctx->CSSetShaderResources(m_bindIdx, 1, srv);
    }
    else
    {
        ID3D11UnorderedAccessView *const uav[1] = { NULL };
        ctx->CSSetUnorderedAccessViews(0, 1, uav, NULL);
    }
}

void WinDX11Texture::Bind(Renderer* renderer)
{
    WinDX11Renderer* driver = (WinDX11Renderer*)renderer;
    auto ctx = driver->GetImmediateContext();

    if(m_allowReadWrite)
    {
        if(m_unorderedAccessViews[m_rwIdx])
            ctx->CSSetUnorderedAccessViews(0, 1, &m_unorderedAccessViews[m_rwIdx], NULL);

        return;
    }

    ctx->VSSetShaderResources(m_bindIdx, 1, &m_resourceView);
    ctx->PSSetShaderResources(m_bindIdx, 1, &m_resourceView);
    ctx->CSSetShaderResources(m_bindIdx, 1, &m_resourceView);
}

void WinDX11Texture::SetSRV(ID3D11ShaderResourceView* srv)
{
    m_resourceView = srv;
}

void WinDX11Texture::SetUAV(ID3D11UnorderedAccessView* uav)
{
    m_unorderedAccessViews = new ID3D11UnorderedAccessView*[1];
    m_unorderedAccessViews[0] = uav;
}

void* WinDX11Texture::GetTextureSRV()
{
    return m_resourceView;
}
    
}

