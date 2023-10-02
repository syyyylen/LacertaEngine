cbuffer CBuffer : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
    float3 CameraPosition;
    float Time;
    float Ambient;
    float3 LightDirection;
};