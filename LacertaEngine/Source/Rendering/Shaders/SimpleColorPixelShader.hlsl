#include "ScreenShadersLayouts.hlsli"

float4 main(VertexOutput input) : SV_Target
{
    float red = input.position.x / 1920.0f;
    float green = input.position.y / 1080.0f;
    float blue = 0.5f;

    return float4(red, green, blue, 1.0f);
    
    return float4(1.0f, 1.0f, 1.0f, 1.0f );
}
