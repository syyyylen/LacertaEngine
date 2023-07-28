#include "ScreenShadersLayouts.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput ret;
    
    ret.position = float4(input.vertex, 1.0);
    
    return ret;
}
