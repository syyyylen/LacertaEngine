#include "MeshShadersLayouts.hlsli"
#include "ConstantBuffer.hlsli"
#include "MeshConstantBuffer.hlsli"

VertexOutput main(VertexInput input)
{
    VertexOutput ret;

    ret.position = mul(input.position, Local);
    ret.position = mul(ret.position, World);
    ret.position = mul(ret.position, View);
    ret.position = mul(ret.position, Projection);
    ret.texcoord = input.texcoord;
    ret.normal = input.normal;
    ret.directionToCamera = normalize(ret.position.xyz - CameraPosition.xyz);
    
    return ret;
}