#pragma once

#include "../Core.h"
#include "Renderer.h"
#include "../Maths/Maths.h"

#define MAX_LIGHTS 8

namespace LacertaEngine
{

class Material;
    
enum DrawcallType
{
    dcScreen,
    dcMesh
};

struct MatLightProperties
{
    float DiffuseIntensity = 1.0f;
    float SpecularIntensity = 1.0f;
    float Shininess = 10.0f;
};

struct DrawcallData
{
    void* VBO;
    unsigned long VerticesCount;
    void* IBO;
    unsigned long IndicesCount;
    DrawcallType Type;
    Matrix4x4 LocalMatrix;
    Material* Material;
};


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
struct ConstantBuffer
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
    PointLight PointLights[MAX_LIGHTS];
};

__declspec(align(16))
struct MeshConstantBuffer
{
    Matrix4x4 LocalMatrix;
    MatLightProperties LightProperties;
    int HasAlbedo;
    int HasNormalMap;
};

struct VertexDataScreen
{
    Vector3 Position;
};

struct VertexMesh
{
    Vector3 Position;
    Vector2 Texcoord;
    Vector3 Normal;
    Vector3 Tangent;
    Vector3 Binormal;
};

class Shader;
    
class LACERTAENGINE_API Drawcall
{
public:
    Drawcall();
    virtual ~Drawcall();

    virtual void Setup(Renderer* renderer, DrawcallData* dcData) = 0;
    virtual void Pass(Renderer* renderer) = 0;
    virtual void CreateVBO(Renderer* renderer, void* data, unsigned long size) = 0;
    virtual void CreateIBO(Renderer* renderer, void* data, unsigned long size) = 0;

    virtual void* GetVBO() = 0;
    virtual void* GetIBO() = 0;
    unsigned long GetVerticesCount() { return m_verticesCount; }
    unsigned long GetIndexListSize() { return m_indexCount; }
    DrawcallType GetType() { return m_type; }
    Matrix4x4 LocalMatrix() { return m_localMatrix; } // TODO relocate this
    Material* GetMaterial() { return m_material; }

protected:
    Shader* m_shader;
    unsigned long m_verticesCount;
    unsigned long m_indexCount;
    DrawcallType m_type;
    Matrix4x4 m_localMatrix;
    Material* m_material;
};

}
