// Texture2D BaseColor: register(t0);
// Texture2D NormalMap: register(t1);
// TextureCube SkyBox: register(t2);
// TextureCube IrradianceMap: register(t3);
// Texture2D RoughnessMap: register(t4);
// Texture2D MetallicMap: register(t5);
// Texture2D AmbiantOcclusionMap: register(t6);
// Texture2D BRDFLut: register(t7);

Texture2D BaseColor: register(t0);
Texture2D NormalMap: register(t1);
Texture2D RoughnessMap: register(t2);
Texture2D MetallicMap: register(t3);
Texture2D AmbiantOcclusionMap: register(t4);

sampler TextureSampler: register(s0);

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