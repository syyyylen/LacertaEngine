#include "Texture.h"

LacertaEngine::Texture::Texture()
{
}

LacertaEngine::Texture::~Texture()
{
}

void LacertaEngine::Texture::AllowReadWrite(Renderer* renderer, bool allow, int rwIdx)
{
    m_allowReadWrite = allow;
    m_rwIdx = rwIdx;
    OnReadWriteAccessChanged(renderer);
}

bool LacertaEngine::Texture::HasTextureFlag(int val, TextureBindFlags flag)
{
    return val & flag;
}
