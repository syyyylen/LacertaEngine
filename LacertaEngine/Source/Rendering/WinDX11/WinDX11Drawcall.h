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

    void Setup(Renderer* renderer) override;
    void Pass(Renderer* renderer) override;
    void CreateVBO(Renderer* renderer, void* data, unsigned long size) override;
    void* GetVBO() override { return m_vbo; }

private:
    ID3D11Buffer* m_vbo;
};

}
