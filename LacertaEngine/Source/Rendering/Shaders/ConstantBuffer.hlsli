#define MAX_LIGHTS 8

struct PointLight
{
    float3 Position;
    float Padding1;
    // 16 bytes boundary 
    float4 Color;
    // 16 bytes boundary 
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    float Padding3;
    // 16 bytes boundary
    bool Enabled;
    float3 Padding4;
    // 16 bytes boundary
}; // size 48 bytes (16 * 3)

cbuffer CBuffer : register(b0)
{
    row_major float4x4 World; 
    row_major float4x4 View;
    row_major float4x4 Projection;
    // 192 bytes (3 * 64 bytes)
    float3 CameraPosition;
    float Time;
    // 16 bytes boundary 
    float GlobalAmbient;
    float3 DirectionalLightDirection;
    // 16 bytes boundary
    PointLight PointLights[MAX_LIGHTS]; // 348 bytes (8 * 48 bytes)
}; // size 572 bytes (348 + 16 + 16 + 192)