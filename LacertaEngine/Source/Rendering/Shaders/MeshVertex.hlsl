#include "SceneMeshShaderLayouts.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput ret;

    ret.position = mul(input.position, Local);
    float4 p = ret.position;
    ret.position = mul(ret.position, World);
    ret.positionWS = ret.position.xyz;
    ret.viewVector = normalize(ret.position.xyz - CameraPosition.xyz);
    ret.position = mul(ret.position, View);
    ret.position = mul(ret.position, Projection);
    ret.texcoord = input.texcoord;
    ret.normal = normalize(mul(input.normal, (float3x3)World));
    
    ret.tbn[0] = normalize(mul(input.tangent, World));
    ret.tbn[1] = normalize(mul(input.binormal, World));
    ret.tbn[2] = normalize(mul(input.normal, World));

    ret.lightSpacePos = mul(p, SMLightView);
    ret.lightSpacePos = mul(ret.lightSpacePos, SMLightProjection);
    
    return ret;
}