#pragma once

#include "Drawcall.h"
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Material
{
public:
    Material();
    ~Material();

    void InitializeProperties(MatLightProperties properties, std::string shaderName, Texture* baseColor);
    void InitializeProperties(MatLightProperties properties, std::string shaderName);
     
    void SetMatLightProperties(MatLightProperties properties) { m_lightProperties = properties; }
    MatLightProperties GetMatLightProperties() { return m_lightProperties; }

    void SetShader(std::string shaderName) { m_shaderName = shaderName; }
    std::string GetShader() { return m_shaderName; }

    void SetTexture(size_t index, Texture* texture);

    Texture* GetTexture(size_t index)
    {
        if (index < m_textures.size())
            return m_textures[index];
        else
            return nullptr;
    }

    const std::vector<Texture*>& GetTextures() { return m_textures; }

    bool CastShadow() { return m_castShadow; }
    void SetCastShadow(bool cast) { m_castShadow = cast; }
    
private:
    MatLightProperties m_lightProperties;
    std::string m_shaderName;
    std::vector<Texture*> m_textures;
    bool m_castShadow = true;
};

}
