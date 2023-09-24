#include "Texture.h"

#include "../../Logger/Logger.h"
#include "../../Rendering/GraphicsEngine.h"

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
    LOG(Error, "CreateResource called on Texture base class instead of Renderer subclass");
}

}
