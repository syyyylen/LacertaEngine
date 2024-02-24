// ------------------------------------------- Textures & Sampler -------------------------------------------
TextureCube SkyBox: register(t5);
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

struct SkyboxVertexOutput
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD0;
};

// ------------------------------------------- Global CBuffer -------------------------------------------
cbuffer SkyBoxCBuffer : register(b2)
{
    row_major float4x4 View;
    row_major float4x4 Projection;
    float GlobalAmbient;
    float Padding[3];
}