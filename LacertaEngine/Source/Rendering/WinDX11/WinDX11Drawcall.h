#pragma once
#include <d3d11.h>

#include "../Drawcall.h"
#include "../../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API WinDX11Drawcall : public Drawcall
{
public:
    WinDX11Drawcall();
    ~WinDX11Drawcall();

    void Setup(Renderer* renderer, DrawcallType type, const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath) override;
    void Pass(Renderer* renderer) override;
    void CreateVBO(Renderer* renderer, void* data, unsigned long size) override;
    void CreateIBO(Renderer* renderer, void* data, unsigned long size) override;
    void* GetVBO() override { return m_vbo; }
    void* GetIBO() override { return m_ibo; }

private:
    ID3D11Buffer* m_vbo;
    ID3D11Buffer* m_ibo; 
};

}
