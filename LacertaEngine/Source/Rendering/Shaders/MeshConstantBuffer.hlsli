Texture2D Texture: register(t0);
sampler TextureSampler: register(s0);

struct MaterialLightProperties
{
    float DiffuseIntensity;
    float SpecularIntensity;
    float Shininess;
};

cbuffer CBuffer : register(b1)
{
    row_major float4x4 Local;
    MaterialLightProperties MatLightProperties;
};