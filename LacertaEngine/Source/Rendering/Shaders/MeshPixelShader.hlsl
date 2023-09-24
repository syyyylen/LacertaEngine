#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

float4 main(VertexOutput input) : SV_Target
{
    return Texture.Sample(TextureSampler, input.texcoord * 0.5f);
    
    float3 normalColor = 0.5f * (input.normal + 1.0f);
    return float4(normalColor, 1.0f); 
    
    return float4(255.0f, 240.0f, 0.0f, 1.0f);
}