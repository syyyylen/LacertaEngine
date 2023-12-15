#pragma once

#include "../Core.h"
#include "../RessourcesManager/Resources.h"

namespace LacertaEngine
{
    
class SkyBoxTexture;

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

    // TODO clean this
    void SetIsSkyBox(bool skyBox) { m_isSkyBox = skyBox; }
    bool IsSkyBox() const { return m_isSkyBox; }
    SkyBoxTexture* GetSkyBoxTex() { return m_skyBoxTex; }
    void SetSkyBoxTexture(SkyBoxTexture* tex) { m_skyBoxTex = tex; }
    // TODO clean this

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

    SkyBoxTexture* m_skyBoxTex = nullptr; // TODO clean this plz
    bool m_isSkyBox = false; // TODO this is beyond cringe
};

}
