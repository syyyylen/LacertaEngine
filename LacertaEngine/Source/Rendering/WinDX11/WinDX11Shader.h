#pragma once
#include <d3d11.h>

#include "../Shader.h"
#include "../../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API WinDX11Shader : public Shader
{
public:
    WinDX11Shader();
    ~WinDX11Shader();

    void Load(Renderer* renderer, DrawcallType Type) override;
    void PreparePass(Renderer* renderer) override;
    void Pass(Renderer* renderer, Drawcall* dc) override;

    unsigned int GetVerticesStride() { return m_vertexLayoutStride; }

protected:
    bool m_loaded;
    
private:
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_fragmentShader;
    ID3D11InputLayout* m_vertexLayout;
    
    unsigned int m_vertexLayoutStride;
};

}
