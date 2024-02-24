#include "SkyBoxShaderLayouts.hlsli"

SkyboxVertexOutput main(VertexInput input)
{
    SkyboxVertexOutput ret;

    ret.texcoord = input.position;
    float4x4 newView = View;
    newView[3][0] = 0.0;
    newView[3][1] = 0.0;
    newView[3][2] = 0.0;
	
    input.position.w = 1.0f;
    ret.position = mul(input.position, newView);
    ret.position = mul(ret.position, Projection);

    ret.position = ret.position.xyzw;
    ret.position.z = ret.position.w * 0.9999;

    return ret;
}