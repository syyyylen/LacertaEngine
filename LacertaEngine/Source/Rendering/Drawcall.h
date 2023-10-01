﻿#pragma once

#include "../Core.h"
#include "Renderer.h"
#include "../Maths/Maths.h"
#include "../RessourcesManager/Texture/Texture.h"

namespace LacertaEngine
{

enum DrawcallType
{
    dcScreen,
    dcMesh
};

struct MatLightProperties
{
    Vector4 DiffuseColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    Vector4 SpecularColor =  Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    float SpecularIntensity = 16.0f;
};

struct DrawcallData
{
    void* VBO;
    unsigned long VerticesCount;
    void* IBO;
    unsigned long IndicesCount;
    Texture* Texture;
    std::string ShaderName;
    DrawcallType Type;
    Matrix4x4 LocalMatrix;
    MatLightProperties LightProperties;
};

__declspec(align(16))
struct ConstantBuffer
{
    Matrix4x4 WorldMatrix;
    Matrix4x4 ViewMatrix;
    Matrix4x4 ProjectionMatrix;
    Vector3 CameraPosition;
    float Time = 0.0f;
    float Ambient = 0.1f; // TODO all those light constants will become material relative
    float Diffuse = 1.0f;
    float Specular = 1.0f;
    float Shininess = 30.0f;
    Vector3 LightDirection;
};

__declspec(align(16))
struct MeshConstantBuffer
{
    Matrix4x4 LocalMatrix;
    MatLightProperties LightProperties;
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
    MatLightProperties LigthProperties() { return m_lightProperties; } // TODO relocate this
    Texture* GetTexture() { return m_texture; }

protected:
    Texture* m_texture;
    Shader* m_shader;
    unsigned long m_verticesCount;
    unsigned long m_indexCount;
    DrawcallType m_type;

    // TODO the way the info is transferred all across the pipeline seems stupid af 
    Matrix4x4 m_localMatrix;
    MatLightProperties m_lightProperties;
};

}
