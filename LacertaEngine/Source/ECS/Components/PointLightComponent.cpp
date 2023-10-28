#include "PointLightComponent.h"

namespace LacertaEngine
{
    
PointLightComponent::PointLightComponent()
{
}

PointLightComponent::~PointLightComponent()
{
}

void PointLightComponent::SetAttenuation(float constant, float linear, float quadratic)
{
    m_constantAttenuation = constant;
    m_linearAttenuation = linear;
    m_quadraticAttenuation = quadratic;
}
}
