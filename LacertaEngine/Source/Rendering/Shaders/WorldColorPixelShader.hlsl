#include "WorldShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"

float4 main(VertexOutput input) : SV_Target
{
    return float4(lerp(input.color, input.color1, (float)((sin((float)(Time / (float)500.0f)) + 1.0f) / 2.0f)),1.0f);
    
    return float4(255.0f, 240.0f, 0.0f, 1.0f);
    
    float red = input.position.x / 1920.0f;
    float green = input.position.y / 1080.0f;
    float blue = 0.5f;
    
    float frequency = 1.6f; 
    float amplitude = 0.25f; 
    
    float timeFactor = sin(Time * frequency) * amplitude;
    
    red += timeFactor;
    green += timeFactor;
    blue += timeFactor;
    
    red = saturate(red);
    green = saturate(green);
    blue = saturate(blue);
    
    return float4(red, green, blue, 1.0f);
    
    // return float4(1.0f, 1.0f, 1.0f, 1.0f );
}
