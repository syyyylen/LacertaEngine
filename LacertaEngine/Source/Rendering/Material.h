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

    void InitializeProperties(MatLightProperties properties, std::string shaderName, Texture* texture);
    
    void SetMatLightProperties(MatLightProperties properties) { m_lightProperties = properties; }
    MatLightProperties GetMatLightProperties() { return m_lightProperties; }

    void SetShader(std::string shaderName) { m_shaderName = shaderName; }
    std::string GetShader() { return m_shaderName; }

    void SetTexture(Texture* texture) { m_texture = texture; }
    Texture* GetTexture() { return m_texture; }
    
private:
    MatLightProperties m_lightProperties;
    std::string m_shaderName;
    Texture* m_texture;
};

}
