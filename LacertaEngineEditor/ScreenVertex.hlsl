// #include "ScreenShadersLayouts.hlsli"

// struct VertexInput
// {
//     float3 vertex : POSITION;
// };
//
// struct VertexOutput
// {
//     float4 position : SV_POSITION; 
// };

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}