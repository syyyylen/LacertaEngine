#include "WorldShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput ret;

    ret.position = mul(input.vertex, World);
    ret.position = mul(ret.position, View);
    ret.position = mul(ret.position, Projection);
    ret.color = input.color;
    ret.color1 = input.color1;

    return ret;
}