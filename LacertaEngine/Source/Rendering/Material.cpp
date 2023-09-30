#include "Material.h"

LacertaEngine::Material::Material()
{
}

LacertaEngine::Material::~Material()
{
}

void LacertaEngine::Material::InitializeProperties(MatLightProperties properties, std::string shaderName, Texture* texture)
{
    SetMatLightProperties(properties);
    SetShader(shaderName);
    SetTexture(texture);
}
