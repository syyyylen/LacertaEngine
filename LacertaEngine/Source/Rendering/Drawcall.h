#pragma once

#include "Bindable.h"
#include "Drawable.h"
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
    float Shininess = 0.0f;
    float Metallic = 0.0f;
    // 16 bytes
    float Roughness = 0.6f;
    float Padding1[3];
    // 16 bytes
}; // 32 bytes

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
    float DirectionalIntensity = 1.0f;
    float padding1[3];
    // 16 bytes boundary
    Vector4 DefaultColor;
    // 16 bytes boundary
    PointLight PointLights[MAX_LIGHTS];
};

__declspec(align(16))
struct MeshConstantBuffer
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
    Drawcall(std::string shaderName, Drawable* drawable, std::list<Bindable*> bindables);
    virtual ~Drawcall();

    void PreparePass(Renderer* renderer);
    void Pass(Renderer* renderer);
    Drawable* GetDrawable() { return m_drawable; }

private:
    std::string m_shaderName;
    Shader* m_shader;
    Drawable* m_drawable;
    std::list<Bindable*> m_bindables;
};

}
