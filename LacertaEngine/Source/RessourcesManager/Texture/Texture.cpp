#include "Texture.h"

#include "../../Logger/Logger.h"

namespace LacertaEngine
{

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::CreateResource(const wchar_t* filePath)
{
    LOG(Error, "CreateResource was called on Texture base class, it should only be called on implementations");
}

}