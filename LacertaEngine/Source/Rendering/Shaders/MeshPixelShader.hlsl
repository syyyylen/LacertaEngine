#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

float3 DoDiffuse(float4 lightColor, float3 lightDirection, float3 normal)
{
    return max(0.0f, dot(lightDirection, normal)) * lightColor * MatLightProperties.DiffuseIntensity;
}

float3 DoSpecular(float4 lightColor, float3 lightDirection, float3 normal, float3 view, float3 inputNormal)
{
    float3 reflectedLight = normalize(reflect(lightDirection, normal));
    float amountSpecularLight = 0;
    if(dot(lightDirection, inputNormal) > 0) // we use vertex normal instead of normal one
        amountSpecularLight = pow(max(0.0f, dot(reflectedLight, view)), MatLightProperties.Shininess);

    return  MatLightProperties.SpecularIntensity * amountSpecularLight *  lightColor;
}

float DoAttenuation(PointLight light, float distance)
{
    return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * distance + light.QuadraticAttenuation * distance * distance);
}

float4 main(VertexOutput input) : SV_Target
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f); // default white color
    float3 normal = input.normal;

    if(HasNormalMap)
    {
        float4 normalSampled = NormalMap.Sample(NormalSampler, float2(input.texcoord.x, 1.0 - input.texcoord.y));
        normalSampled.xyz = (normalSampled.xyz * 2.0) - 1.0;
        normalSampled.xyz = mul(normalSampled.xyz, input.tbn);
        normal = normalSampled.xyz;
    }

    if(HasAlbedo)
        color = BaseColor.Sample(TextureSampler,  float2(input.texcoord.x, 1.0 - input.texcoord.y));

    float3 ambiantLight = GlobalAmbient * float3(color.x, color.y, color.z); // Ambient is texture color
    float3 diffuseLight = DoDiffuse(float4(1.0, 1.0, 1.0, 1.0), DirectionalLightDirection, normal);
    float specularLight = DoSpecular(float4(1.0, 1.0, 1.0, 1.0), DirectionalLightDirection, normal, input.viewVector, input.normal);

    float3 finalLight = ambiantLight + diffuseLight + specularLight;

    // test 
    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        PointLight light = PointLights[i];
        
        if(!light.Enabled)
            continue;
        
        float3 l = (light.Position - input.positionWS);
        float3 lightDirection = normalize(l);
        float distance = length(l);
        
        float attenuation = DoAttenuation(light, distance);
        float3 diffuse = DoDiffuse(light.Color, lightDirection, normal) * attenuation;
        float3 specular = DoSpecular(light.Color, lightDirection, normal, input.viewVector, input.normal) * attenuation;

        finalLight += diffuse + specular;
    }
    
    return color * float4(finalLight, color.a);

    // safety Yellow
    // return float4(255.0f, 240.0f, 0.0f, 1.0f);
}