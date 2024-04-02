#include "SceneMeshShaderLayouts.hlsli"

static const float PI = 3.1415926;
static const float Epsilon = 0.00001;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

float DoAttenuation(PointLight light, float distance)
{
    return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * distance + light.QuadraticAttenuation * distance * distance);
}

float DistributionGGX(float3 N, float3 H, float a)
{
    a = a * a;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / max(denom, 0.00001f);
}

float GeometryShlickGGX(float a, float3 N, float3 X)
{
    float nom = max(dot(N, X), 0.0f);
    
    float r = (a + 1.0);
    float k = (r*r) / 8.0;
    
    float denom = max(dot(N, X), 0.0f) * (1.0f - k) + k;
    denom = max(denom, 0.00001f);

    return nom / denom;
}

float GeometrySmith(float a, float3 N, float3 V, float3 L)
{
    return GeometryShlickGGX(a, N, V) * GeometryShlickGGX(a, N, L);
}

float3 FresnelShlick(float3 F0, float3 V, float3 H)
{
    return F0 + (1.0f - F0) * pow(1.0f - max(dot(V, H), 0.0f), 5.0f);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float r)   // cosTheta is n.v
{
    return F0 + (max(float3(1.0f - r, 1.0f - r, 1.0f - r), F0) - F0) * pow(1.0 - cosTheta, 5.0f);
}

float3 PBR(float3 F0, float3 N, float3 V, float3 L, float3 H, float3 radiance, float3 albedo, float roughness, float metallic)
{
    float3 Ks = FresnelShlick(F0, V, H);
    float3 Kd = float3(1.0f, 1.0f, 1.0f) - Ks;
    Kd *= 1.0 - metallic;
    
    float3 lambert = albedo / PI;
    
    float3 cookTorranceNumerator = DistributionGGX(N, H, roughness) * GeometrySmith(roughness, N, V, L) * FresnelShlick(F0, V, H);
    float cookTorranceDenominator = 4.0f * max(dot(V, N), 0.0f) * max(dot(L, N), 0.0f);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.00001f);
    float3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;
    
    float3 BRDF = Kd * lambert + cookTorrance;
    float3 outgoingLight = BRDF * radiance * max(dot(L, N), 0.0f);
    
    return outgoingLight;
}

float2 TexOffset(int u, int v)
{
    return float2( u * 1.0f/2048.0f, v * 1.0f/2048.0f);
}

float4 main(VertexOutput input) : SV_Target
{
    float4 directionalColor = float4(1.0f, 1.0f, 1.0f, 1.0f) * DirectionalIntensity; // default white color
    float3 normal = input.normal;
    float2 uv = float2(input.texcoord.x, 1.0 - input.texcoord.y);

    float shadowFactor = 1.0;
    input.lightSpacePos.xyz /= input.lightSpacePos.w;

    if(!(input.lightSpacePos.x < -1.0f || input.lightSpacePos.x > 1.0f || input.lightSpacePos.y < -1.0f || input.lightSpacePos.y > 1.0f || input.lightSpacePos.z < 0.0f  || input.lightSpacePos.z > 1.0f))
    {
        input.lightSpacePos.x = input.lightSpacePos.x/2 + 0.5;
        input.lightSpacePos.y = input.lightSpacePos.y/-2 + 0.5;
        input.lightSpacePos.z -= 0.001f;
        
        float sum = 0;
        float x, y;
        for (y = -1.5; y <= 1.5; y += 1.0)
        {
            for (x = -1.5; x <= 1.5; x += 1.0)
            {
                float2 coord = float2(input.lightSpacePos.xy + TexOffset(x,y));
                sum += ShadowMap.SampleCmpLevelZero(ComparisonSampler, float3(coord.x, coord.y, 0 /* tex idx */), input.lightSpacePos.z);
            }
        }

        shadowFactor = sum / 16.0f;
    }
    
    if(HasNormalMap)
    {
        float4 normalSampled = NormalMap.Sample(TextureSampler, uv);
        normalSampled.xyz = (normalSampled.xyz * 2.0) - 1.0;
        normalSampled.xyz = mul(normalSampled.xyz, input.tbn);
        normal = normalSampled.xyz;
    }

    normal = normalize(normal);

    float3 albedo = DefaultColor.rgb;
    if(HasAlbedo)
        albedo = BaseColor.Sample(TextureSampler, uv).rgb;

    float roughness = MatLightProperties.Roughness;
    if(HasRoughness)
        roughness = RoughnessMap.Sample(TextureSampler, uv);

    float metallic = MatLightProperties.Metallic;
    if(HasMetallic)
        metallic = MetallicMap.Sample(TextureSampler, uv);

    float ao = 1.0f;
    if(HasAmbiant)
        ao = AmbiantOcclusionMap.Sample(TextureSampler, uv);

    // Fresnel reflectance at normal incidence (for metals use albedo color).
    float3 F0 = lerp(Fdielectric, albedo, metallic);
    
    float3 v = normalize(CameraPosition - input.positionWS);
    float3 dirl = DirectionalLightDirection * -1.0f;
    
    // Directional light 
    float3 finalLight = PBR(F0, normal, v, dirl, normalize(dirl + v), directionalColor.xyz, albedo, roughness, metallic);

    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        PointLight light = PointLights[i];
        
        if(!light.Enabled)
            continue;
        
        float3 l = (light.Position - input.positionWS);
        float3 lnorm = normalize(l);
        float distance = length(l);
        float attenuation = DoAttenuation(light, distance);
        float3 radiance = light.Color * attenuation;

        finalLight += PBR(F0, normal, v, lnorm, normalize(lnorm + v), radiance, albedo, roughness, metallic);
    }

    float3 Ks = FresnelSchlickRoughness(max(dot(normal, v), 0.0f), F0,  roughness);
    
    float3 Kd = 1.0f - Ks;
    Kd *= 1.0 - metallic;
    float3 r = reflect(-v, normal);
    float3 irradiance = float3(IrradianceMap.Sample(TextureSampler, r).rgb);
    float3 diffuse = albedo * irradiance;
    
    float3 enviro = float4(SkyBox.Sample(TextureSampler, r));
    float3 env = lerp(enviro, irradiance, roughness);
    // float2 envBRDF = BRDFLut.Sample(TextureSampler, float2(max(dot(normal, v), 0.0), roughness)).rg;
    float3 specular = env * (Ks /* * envBRDF.x + envBRDF.y */);
    
    float3 ambiantLight = (Kd * diffuse + specular) * ao;
    
    // if(isInShadow)
    //     return float4(ambiantLight, 1.0);
    
    float3 l = (ambiantLight * GlobalAmbient) + finalLight * shadowFactor;
    return float4(l, 1.0);
    
    // finalLight += ambiantLight * GlobalAmbient;
    
    return float4(finalLight, 1.0);
}