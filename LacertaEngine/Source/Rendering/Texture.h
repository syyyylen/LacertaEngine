#pragma once
#include "Bindable.h"
#include "GraphicsResource.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Texture : public GraphicsResource, public Bindable
{
public:
    Texture();
    ~Texture();

    virtual void CreateResource(const wchar_t* filePath, Renderer* renderer) = 0;
    virtual void SetTextureIdx(int idx) { m_idx = idx; }
    virtual void Bind(Renderer* renderer) = 0;
    virtual void* GetTextureSRV() = 0;

    void SetNumTexs(int num) { m_numTex = num; }
    bool IsTexArray() { return m_numTex > 1; }

protected:
    int m_idx;
    int m_numTex = 1;
};
    
}

