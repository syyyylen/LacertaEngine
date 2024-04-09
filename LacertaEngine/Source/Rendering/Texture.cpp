#include "Texture.h"

LacertaEngine::Texture::Texture()
{
}

LacertaEngine::Texture::~Texture()
{
}

void LacertaEngine::Texture::AllowReadWrite(Renderer* renderer, bool allow)
{
    m_allowReadWrite = allow;
    OnReadWriteAccessChanged(renderer);
}

bool LacertaEngine::Texture::HasTextureFlag(int val, TextureBindFlags flag)
{
    return val & flag;
}
