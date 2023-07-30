struct VertexInput
{
    float3 vertex : POSITION;
    float3 color : COLOR;
    float3 color1 : COLOR1;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
    float3 color1 : COLOR1;
};