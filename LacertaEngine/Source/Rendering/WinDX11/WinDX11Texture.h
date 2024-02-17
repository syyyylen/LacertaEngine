#pragma once
#include <d3d11.h>

#include "../Texture.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API WinDX11Texture : public Texture
{
public:
    WinDX11Texture();
    ~WinDX11Texture();

    void CreateResource(const wchar_t* filePath, Renderer* renderer) override;

    // TODO remove
    void* GetResource() override;
    void* GetResourceView() override;

private:
    ID3D11Resource* m_resource;
    ID3D11ShaderResourceView* m_resourceView;
};

}
