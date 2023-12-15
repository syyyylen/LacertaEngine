#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

float4 main(VertexOutput input) : SV_Target
{
    float3 sampleV = input.positionWS;
    float4 texColor = SkyBox.Sample(SkyBoxSampler, sampleV);
    texColor *= clamp(DirectionalIntensity, 0.0f, 1.0f);
    return texColor;
}