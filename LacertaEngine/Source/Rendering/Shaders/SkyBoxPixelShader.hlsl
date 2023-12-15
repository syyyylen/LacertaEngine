#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

float4 main(VertexOutput input) : SV_Target
{
    float3 sampleV = input.positionWS;

    return SkyBox.Sample(SkyBoxSampler, sampleV);
    
    float4 texColor = BaseColor.Sample(TextureSampler, float2(input.texcoord.x, 1.0 - input.texcoord.y));
    texColor *= clamp(DirectionalIntensity, 0.0f, 1.0f);
    return float4(texColor.xyz, 1.0f);
}