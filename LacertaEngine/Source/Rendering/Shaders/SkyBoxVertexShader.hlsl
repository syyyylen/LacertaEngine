#include "SceneMeshShaderLayouts.hlsli"

struct SkyboxVertexOutput
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD0;
};

SkyboxVertexOutput main(VertexInput input)
{
    SkyboxVertexOutput ret;

    ret.texcoord = input.position;

    ret.position = mul(input.position, Local);
    ret.position = mul(ret.position, World);
    ret.position = mul(ret.position, View);
    ret.position = mul(ret.position, Projection);

    return ret;
}