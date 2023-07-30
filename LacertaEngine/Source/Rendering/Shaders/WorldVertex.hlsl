#include "WorldShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput ret;

    ret.position = mul(input.vertex, World);
    ret.position = mul(ret.position, View);
    ret.position = mul(ret.position, Projection);

    // ret.position = float4(input.vertex, 1.0f);
    ret.color = input.color;
    ret.color1 = input.color1;

    return ret;
}