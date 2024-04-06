#include "SkyBoxShaderLayouts.hlsli"

static const float PI = 3.1415926;

float4 main(SkyboxVertexOutput input) : SV_Target
{
    float3 normal = normalize(input.texcoord);

    float3 irradiance = float3(0.0f, 0.0f, 0.0f);

    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = cross(up, normal);
    up = cross(normal, right);

    float sampleDelta = 0.025f;
    float nrSamples = 0.0f;

    for (float phi = 0.0f; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0f; theta < 0.5 * PI; theta += sampleDelta)
        {
            float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            float3 sampleVec = (tangentSample.x * right) + (tangentSample.y * up) + (tangentSample.z * normal);

            irradiance += SkyBox.Sample(TextureSampler, sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }
    irradiance = PI * irradiance * (1 / nrSamples);

    return float4(irradiance, 1.0f);
}