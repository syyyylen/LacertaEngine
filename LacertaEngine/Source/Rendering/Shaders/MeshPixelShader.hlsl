#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

float4 main(VertexOutput input) : SV_Target
{
    float4 color = Texture.Sample(TextureSampler, input.texcoord * 0.5f);
    
    float ka = Ambient;
    float3 ia = float3(color.x, color.y, color.z); // ambient light color is texture color
    float3 ambiantLight = ka * ia;

    float kd = Diffuse; // diffuse amount
    float id = color; // diffuse color is also texture color for now
    float amountOfDiffuseLight = max(0.0f, dot(LightDirection, input.normal));
    float3 diffuseLight = kd * amountOfDiffuseLight * id;

    float ks = Specular;
    float3 is = MatLightProperties.SpecularColor;
    float3 reflectedLight = reflect(LightDirection, input.normal);
    float shininess = Shininess;
    float amountSpecularLight = pow(max(0.0f, dot(reflectedLight, input.directionToCamera)), shininess);

    float specularLight = ks * amountSpecularLight * is;

    float3 finalLight = ambiantLight + diffuseLight + specularLight;
    
    return color * float4(finalLight, 1.0f);

    // safety Yellow
    // return float4(255.0f, 240.0f, 0.0f, 1.0f);
}