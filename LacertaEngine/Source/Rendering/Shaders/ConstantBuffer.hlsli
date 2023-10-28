#define MAX_LIGHTS 8

struct PointLight
{
    float3 Position;
    float4 Color;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
};

cbuffer CBuffer : register(b0)
{
    row_major float4x4 World;
    row_major float4x4 View;
    row_major float4x4 Projection;
    float3 CameraPosition;
    float Time;
    float GlobalAmbient;
    float3 DirectionalLightDirection;
    PointLight PointLights[MAX_LIGHTS];
};