#pragma once
#include "../Maths/Maths.h"

#define MAX_LIGHTS 8

namespace LacertaEngine
{
    
struct MatLightProperties
{
    float DiffuseIntensity = 1.0f;
    float SpecularIntensity = 1.0f;
    float Shininess = 0.0f;
    float Metallic = 0.0f;
    // 16 bytes
    float Roughness = 0.6f;
    float Padding1[3];
    // 16 bytes
}; // 32 bytes

struct PointLight
{
    Vector3 Position;
    float Padding1 = 0.0f;
    // 16 bytes boundary 
    Vector4 Color;
    // 16 bytes boundary 
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;
    float Padding3;
    // 16 bytes boundary
    int Enabled;
    float Padding4[3];
};

// __declspec(align(16))
struct SceneConstantBuffer
{
    Matrix4x4 WorldMatrix;
    Matrix4x4 ViewMatrix;
    Matrix4x4 ProjectionMatrix;
    // 3 * 64 bytes
    Vector3 CameraPosition;
    float Time = 0.0f;
    // 16 bytes boundary 
    float GlobalAmbient = 0.1f;
    Vector3 DirectionalLightDirection;
    // 16 bytes boundary
    float DirectionalIntensity = 1.0f;
    float padding1[3];
    // 16 bytes boundary
    Vector4 DefaultColor;
    // 16 bytes boundary
    PointLight PointLights[MAX_LIGHTS];
};

__declspec(align(16))
struct SceneMeshConstantBuffer
{
    Matrix4x4 LocalMatrix;
    // 64 bytes
    MatLightProperties LightProperties;
    // 64 + 32 bytes
    int HasAlbedo;
    int HasNormalMap;
    int HasRoughness;
    int HasMetallic;
    // 16 bytes
    int HasAmbiant;
};

struct SceneVertexMesh
{
    Vector3 Position;
    Vector2 Texcoord;
    Vector3 Normal;
    Vector3 Tangent;
    Vector3 Binormal;
};
    
}
