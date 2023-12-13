#pragma once

#include "../Core.h"
#include "../RessourcesManager/Resources.h"

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

    void SetIsSkyBox(bool skyBox) { m_isSkyBox = skyBox; }
    bool IsSkyBox() const { return m_isSkyBox; }

    Texture* GetTexture(size_t index)
    {
        if (index < m_textures.size())
            return m_textures[index];
        else
            return nullptr;
    }
    
private:
    MatLightProperties m_lightProperties;
    std::string m_shaderName;
    std::vector<Texture*> m_textures;
    bool m_isSkyBox = false; // TODO this is beyond cringe
};

}
