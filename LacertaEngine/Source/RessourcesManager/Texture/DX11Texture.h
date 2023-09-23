#pragma once

#include <d3d11.h>
#include "Texture.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API DX11Texture : public Texture
{
public:
    DX11Texture();
    ~DX11Texture();

    void CreateResource(const wchar_t* filePath) final;

private:
    ID3D11Resource* m_texture = nullptr;
    ID3D11ShaderResourceView* m_shaderResView = nullptr;
};

}

