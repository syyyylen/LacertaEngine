#include "Material.h"

namespace LacertaEngine
{
    
Material::Material()
{
}

Material::~Material()
{
}

void Material::InitializeProperties(MatLightProperties properties, std::string shaderName, Texture* baseColor)
{
    SetMatLightProperties(properties);
    SetShader(shaderName);
    SetTexture(0, baseColor);
}

void Material::SetTexture(size_t index, Texture* texture)
{
    if(index < m_textures.size())
    {
        m_textures[index] = texture;
    }
    else
    {
        m_textures.push_back(texture);
    }
}
    
}
