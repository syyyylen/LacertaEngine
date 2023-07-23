// #include "ScreenShadersLayouts.hlsli"

struct VertexInput
{
    float3 vertex : POSITION;
};

struct VertexOutput
{
    float4 position : SV_POSITION; 
};

VertexOutput main(VertexInput IN)
{
    VertexOutput ret;

    ret.position = float4(IN.vertex, 0.0f);

    return ret;
}