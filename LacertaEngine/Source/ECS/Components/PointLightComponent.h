#pragma once
#include "../Component.h"
#include "../../Maths/Maths.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API PointLightComponent : public Component
{
public:
    PointLightComponent();
    ~PointLightComponent();

    void SetColor(Vector4 lightColor) { m_color = lightColor; }
    void SetAttenuation(float constant, float linear, float quadratic);

    float GetConstantAttenuation() const { return m_constantAttenuation; }
    float GetLinearAttenuation() const { return m_linearAttenuation; }
    float GetQuadraticAttenuation() const { return m_quadraticAttenuation; }

private:
    Vector4 m_color;
    float m_constantAttenuation = 1.0f;
    float m_linearAttenuation = 1.0f;
    float m_quadraticAttenuation = 1.0f;
};
    
}
