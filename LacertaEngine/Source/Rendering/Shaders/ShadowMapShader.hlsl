#include "SceneMeshShaderLayouts.hlsli"

struct ShadowMapPixelInput
{
    float4 pos : SV_POSITION;
};

ShadowMapPixelInput main(VertexInput input)
{
    ShadowMapPixelInput ret;
    float4 p = float4(input.position.xyz, 1.0f);

    p = mul(p, Local);
    p = mul(p, SMLightView[0]);
    p = mul(p, SMLightProjection[0]);

    ret.pos = p;

    return ret;
}