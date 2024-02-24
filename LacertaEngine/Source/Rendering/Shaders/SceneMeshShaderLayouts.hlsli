// ------------------------------------------- Textures & Sampler -------------------------------------------
Texture2D BaseColor: register(t0);
Texture2D NormalMap: register(t1);
Texture2D RoughnessMap: register(t2);
Texture2D MetallicMap: register(t3);
Texture2D AmbiantOcclusionMap: register(t4);
TextureCube SkyBox: register(t5); 
TextureCube IrradianceMap: register(t6);
Texture2D BRDFLut: register(t7);

sampler TextureSampler: register(s0);

// ------------------------------------------- VS Input / Output layout -------------------------------------------
struct VertexInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 binormal : BINORMAL0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 positionWS : TEXCOORD0;
    float3 viewVector : TEXCOORD1;
    float2 texcoord : TEXCOORD2;
    float3 normal : NORMAL;
    row_major float3x3 tbn: TEXCOORD3;
};

// ------------------------------------------- Per Instance CBuffer -------------------------------------------
struct MaterialLightProperties
{
    float DiffuseIntensity;
    float SpecularIntensity;
    float Shininess;
    float Metallic;
    // 16 bytes
    float Roughness;
    float3 Padding1;
    // 16 bytes
}; // 32 bytes

cbuffer MeshCBuffer : register(b1)
{
    row_major float4x4 Local;
    // 64 bytes
    MaterialLightProperties MatLightProperties;
    // 64 + 32 bytes
    bool HasAlbedo;
    bool HasNormalMap;
    bool HasRoughness;
    bool HasMetallic;
    bool HasAmbiant;
};


// ------------------------------------------- Global CBuffer -------------------------------------------
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
    float DirectionalIntensity;
    float3 Padding1;
    // 16 bytes boundary
    float4 DefaultColor;
    // 16 bytes boundary
    PointLight PointLights[MAX_LIGHTS]; // 348 bytes (8 * 48 bytes)
}; // size 572 bytes (348 + 16 + 16 + 192)