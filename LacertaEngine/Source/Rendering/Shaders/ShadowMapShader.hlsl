#include "SceneMeshShaderLayouts.hlsli"

struct ShadowMapPixelInput
{
    float4 pos : SV_POSITION;
};

cbuffer ShadowMapLightCBuffer : register(b3)
{
    row_major float4x4 SMLightView;
    row_major float4x4 SMLightProjection;
};

ShadowMapPixelInput main(VertexInput input)
{
    ShadowMapPixelInput ret;
    float4 p = float4(input.position.xyz, 1.0f);

    p = mul(p, Local);
    p = mul(p, SMLightView);
    p = mul(p, SMLightProjection);

    ret.pos = p;

    return ret;
}