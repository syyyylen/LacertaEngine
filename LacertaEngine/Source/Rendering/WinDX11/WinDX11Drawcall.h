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

    void Setup(Renderer* renderer, DrawcallData* dcData) override;
    void Pass(Renderer* renderer) override;
    void* GetVBO() override { return m_vbo; }
    void* GetIBO() override { return m_ibo; }

private:
    ID3D11Buffer* m_vbo;
    ID3D11Buffer* m_ibo;
};

}
