#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

struct SkyboxVertexOutput
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD0;
};

float4 main(SkyboxVertexOutput input) : SV_Target
{
    float4 texColor = SkyBox.Sample(TextureSampler, input.texcoord);
    texColor *= clamp(GlobalAmbient, 0.0f, 1.0f);
    return texColor;
}