#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

float4 main(VertexOutput input) : SV_Target
{
    // return float4(input.texcoord.x, input.texcoord.y, 0, 1);

    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f); // default white color

    if(HasAlbedo)
        color = BaseColor.Sample(TextureSampler, input.texcoord);

    // AMBIENT 
    float ka = Ambient;
    float3 ia = float3(color.x, color.y, color.z); // ambient light color is texture color
    float3 ambiantLight = ka * ia;

    // DIFFUSE 
    float kd = MatLightProperties.DiffuseIntensity; // diffuse amount
    float4 id = color; // diffuse color is also texture color
    float amountOfDiffuseLight = max(0.0f, dot(LightDirection, input.normal));
    float3 diffuseLight = kd * amountOfDiffuseLight * id;

    // SPECULAR 
    float ks = MatLightProperties.SpecularIntensity;
    float3 is = float3(1.0, 1.0, 1.0);
    float3 reflectedLight = normalize(reflect(LightDirection, input.normal));
    float shininess = MatLightProperties.Shininess;
    float amountSpecularLight = pow(max(0.0f, dot(reflectedLight, input.viewVector)), shininess);

    float specularLight = ks * amountSpecularLight * is;

    float3 finalLight = ambiantLight + diffuseLight + specularLight;
    
    return color * float4(finalLight, color.a);

    // safety Yellow
    // return float4(255.0f, 240.0f, 0.0f, 1.0f);
}