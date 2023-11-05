Texture2D BaseColor: register(t0);
sampler TextureSampler: register(s0);

Texture2D NormalMap: register(t1);
sampler NormalSampler: register(s1);

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
};