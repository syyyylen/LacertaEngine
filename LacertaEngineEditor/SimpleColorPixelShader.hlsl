// #include "ScreenShadersLayouts.hlsli"

struct VertexInput
{
    float3 vertex : POSITION;
};

struct VertexOutput
{
    float4 position : SV_POSITION; 
};

float4 main(VertexOutput IN) : SV_TARGET
{
    
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}