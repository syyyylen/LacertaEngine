﻿#pragma once
#include "../Core.h"
#include "Renderer.h"
#include "../Maths/Vector4.h"

namespace LacertaEngine
{

enum RenderTargetType
{
    BackBuffer,
    Texture2D,
    TextureCube
};
    
class LACERTAENGINE_API RenderTarget
{
public:
    RenderTarget();
    virtual ~RenderTarget();

    virtual void Initialize(Renderer* renderer, int width, int height, RenderTargetType renderTargetType) = 0;
    virtual void SetActive(Renderer* renderer) = 0;
    virtual void Clear(Renderer* renderer, Vector4 color) = 0;
    virtual void SetViewportSize(Renderer* renderer, UINT width, UINT height) = 0;
    virtual void Resize(Renderer* renderer, unsigned width, unsigned height) = 0;
    virtual void* GetSRV() = 0;

    bool RenderToTexture() const { return m_renderTargetType == RenderTargetType::Texture2D; }

protected:
    RenderTargetType m_renderTargetType;
    
    int m_width;
    int m_length;
};

}
