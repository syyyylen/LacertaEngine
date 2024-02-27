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
    void Bind(Renderer* renderer) override;
    void SetSRV(ID3D11ShaderResourceView* srv);

private:
    ID3D11ShaderResourceView* m_resourceView;
};

}
