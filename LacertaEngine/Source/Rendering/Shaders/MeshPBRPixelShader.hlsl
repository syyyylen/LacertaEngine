#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

static const float PI = 3.1415926;
static const float Epsilon = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

float DoAttenuation(PointLight light, float distance)
{
    return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * distance + light.QuadraticAttenuation * distance * distance);
}

float3 PBR(float3 F0, float3 N, float3 V, float3 L, float3 H, float3 radiance)
{
    return float3(1.0f, 1.0f, 1.0f);
}

float4 main(VertexOutput input) : SV_Target
{
    float4 directionalColor = float4(1.0f, 1.0f, 1.0f, 1.0f) * DirectionalIntensity; // default white color
    float3 normal = input.normal;

    if(HasNormalMap)
    {
        float4 normalSampled = NormalMap.Sample(NormalSampler, float2(input.texcoord.x, 1.0 - input.texcoord.y));
        normalSampled.xyz = (normalSampled.xyz * 2.0) - 1.0;
        normalSampled.xyz = mul(normalSampled.xyz, input.tbn);
        normal = normalSampled.xyz;
    }

    float4 albedo = DefaultColor;
    
    if(HasAlbedo)
        albedo = BaseColor.Sample(TextureSampler,  float2(input.texcoord.x, 1.0 - input.texcoord.y));

    // Fresnel reflectance at normal incidence (for metals use albedo color).
    float3 F0 = lerp(Fdielectric, albedo, Metallic);
    float3 v = CameraPosition.xyz - input.positionWS;
    float dirl = DirectionalLightDirection * -1.0f;

    // Directional light 
    float3 finalLight = PBR(F0, normal, v, dirl, normalize(v + dirl), directionalColor);
    
    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        PointLight light = PointLights[i];
        
        if(!light.Enabled)
            continue;
        
        float3 l = (light.Position - input.positionWS);
        float3 lnorm = normalize(l);
        float distance = length(l);
        float attenuation = DoAttenuation(light, distance);
        float3 radiance = light.Color * attenuation;

        finalLight += PBR(F0, normal, v, lnorm, normalize(v + lnorm), radiance);
    }

    return float4(finalLight, 1.0);
}