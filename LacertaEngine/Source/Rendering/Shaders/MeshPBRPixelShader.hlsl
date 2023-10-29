#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

#define PI 3.1415926

float NormalDistribution(float alpha, float3 normal, float3 fresnel) // D (GGX/Trowbridge-Reitz Normal Distribution Function)
{
    float numerator = pow(alpha, 2.0f);

    float normalDotFresnel = max(dot(normal, fresnel), 0.0f);
    float denominator = PI * pow(pow(normalDotFresnel, 2.0f) * (pow(alpha, 2.0f) - 1.0f) + 1.0f, 2.0f);
    denominator = max(denominator, 0.000001f);

    return numerator / denominator;
}

float GeometryShadowing(float alpha, float3 normal, float3 v) // G1 (Shlick-Beckmann)
{
    float numerator = max(dot(normal, v), 0.0f);

    float k = alpha / 2.0f;
    float denominator = max(dot(normal, v), 0.0f) * (1.0f - k) + k;
    denominator = max(denominator, 0.000001f);

    return numerator / denominator;
}

float G(float alpha, float3 normal, float3 viewVector, float3 lightVector) // (Smith model)
{
    return GeometryShadowing(alpha, normal, viewVector) * GeometryShadowing(alpha, normal, lightVector);
}

float3 Fresnel(float3 f, float3 viewVector, float3 fresnel) // (Shlick approximation)
{
    return f + (float3(1.0f, 1.0f, 1.0f) - f) * pow(1.0f - max(dot(viewVector, fresnel), 0.0f), 5.0f);
}

float3 PBR(float3 normal, float3 viewVector, float3 lightVector, float3 fresnel, float3 albedo, float3 lightColor)
{
    float alpha = Roughness;
    float metallic = Metallic;
    float3 f0 = Reflectance;
    
    float3 ks = Fresnel(f0, viewVector, fresnel);
    float3 kd = (float3(1.0f, 1.0f, 1.0f) - ks) * (1.0f - metallic);

    float3 lambert = albedo / PI;

    float3 cookTorranceNumerator = NormalDistribution(alpha, normal, fresnel) * G(alpha, normal, viewVector, lightVector) * Fresnel(f0, viewVector, fresnel);
    float cookTorranceDenominator = 4.0f * max(dot(normal, viewVector), 0.0f) * max(dot(normal, lightVector), 0.0f);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.000001f);
    float3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

    float3 BRDF = kd * lambert + cookTorrance;
    float3 outgoingLight = 0.0f /* emissive */ + BRDF * lightColor * max(dot(lightVector, normal), 0.0f);

    return outgoingLight;
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

    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    if(HasAlbedo)
        color = BaseColor.Sample(TextureSampler,  float2(input.texcoord.x, 1.0 - input.texcoord.y));

    float3 finalLight = PBR(normal, input.viewVector, DirectionalLightDirection * -1.0f, normalize(input.viewVector + DirectionalLightDirection * -1.0f), color.xyz, directionalColor);
    
    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        PointLight light = PointLights[i];
        
        if(!light.Enabled)
            continue;
        
        float3 l = (light.Position - input.positionWS);
        float3 lightVector = normalize(l);
        
        finalLight += PBR(normal, input.viewVector, lightVector, normalize(input.viewVector + lightVector), color.xyz, light.Color);
    }

    return color * float4(finalLight, color.a);
}