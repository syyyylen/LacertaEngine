struct VertexInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 viewVector : TEXCOORD1;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL;
};