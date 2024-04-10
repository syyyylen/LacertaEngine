#pragma once
#include "Bindable.h"
#include "GraphicsResource.h"

namespace LacertaEngine
{

enum TextureType
{
    Tex2D,
    Tex2DArray,
    TexCube
};

enum TextureBindFlags
{
    SRV = 1 << 0,
    RTV = 1 << 1,
    UAV = 1 << 2
};
    
class LACERTAENGINE_API Texture : public GraphicsResource, public Bindable
{
public:
    Texture();
    ~Texture();

    virtual void CreateResource(const wchar_t* filePath, Renderer* renderer) = 0;
    virtual void* Create(Renderer* renderer, int width, int height, TextureType type, int num, int mipNum, int bindFlags) = 0;
    virtual void Bind(Renderer* renderer) = 0;
    virtual void* GetTextureSRV() = 0;
    virtual void OnReadWriteAccessChanged(Renderer* renderer) = 0;

    void SetTextureIdx(int idx) { m_bindIdx = idx; }
    void SetNumTexs(int numTex) { m_numTex = numTex; }
    void AllowReadWrite(Renderer* renderer, bool allow);

    bool HasTextureFlag(int val, TextureBindFlags flag);

protected:
    int m_bindIdx;
    int m_numTex = 1;
    int m_mipMapsLevels = 1;
    bool m_allowReadWrite;
};
    
}

