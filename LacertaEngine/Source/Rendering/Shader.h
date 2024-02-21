#pragma once
#include "Renderer.h"
#include "Drawcall.h"
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Shader
{
public:
    Shader();
    virtual ~Shader();

    virtual void Load(Renderer* renderer, DrawableLayout layout) = 0;
    virtual void PreparePass(Renderer* renderer) = 0;
    virtual void Pass(Renderer* renderer, Drawcall* dc) = 0;

    void SetVSData(void* vsByteCode, size_t vsByteCodeSize)
    {
        m_vertexShaderByteCode = vsByteCode;
        m_vertexByteCodeSize = vsByteCodeSize;
    }

    void SetPSData(void* psByteCode, size_t psByteCodeSize)
    {
        m_pixelShaderByteCode = psByteCode;
        m_pixelByteCodeSize = psByteCodeSize;
    }

protected:
    void* m_vertexShaderByteCode;
    size_t m_vertexByteCodeSize;
    void* m_pixelShaderByteCode;
    size_t m_pixelByteCodeSize;
};

}
