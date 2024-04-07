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
