struct VertexInput
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float3 binormal : BINORMAL0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 positionWS : TEXCOORD0;
    float3 viewVector : TEXCOORD1;
    float2 texcoord : TEXCOORD2;
    float3 normal : NORMAL;
    row_major float3x3 tbn: TEXCOORD3;
};