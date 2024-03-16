struct ShadowMapPixelInput
{
    float4 pos : SV_POSITION;
};

float4 main(ShadowMapPixelInput input) : SV_Target
{
    return float4(255.0f, 240.0f, 0.0f, 1.0f);
}