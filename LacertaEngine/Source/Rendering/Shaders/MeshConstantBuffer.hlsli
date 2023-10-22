Texture2D BaseColor: register(t0);
sampler TextureSampler: register(s0);

struct MaterialLightProperties
{
    float DiffuseIntensity;
    float SpecularIntensity;
    float Shininess;
};

cbuffer MeshCBuffer : register(b1)
{
    row_major float4x4 Local;
    MaterialLightProperties MatLightProperties;
    bool HasAlbedo;
};