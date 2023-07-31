#pragma once
#include "../Core.h"
#include "Renderer.h"
#include "../Maths/Maths.h"

namespace LacertaEngine
{

enum DrawcallType
{
    Screen,
    Mesh
};

struct DrawcallData
{
    void* Data;
    unsigned long Size;
    const wchar_t* VertexShaderPath;
    const wchar_t* PixelShaderPath;
    DrawcallType Type;
    void* IndexesData;
    unsigned long IndexesSize;
    Matrix4x4 LocalMatrix;
};

__declspec(align(16))
struct ConstantBuffer
{
    Matrix4x4 WorldMatrix;
    Matrix4x4 ViewMatrix;
    Matrix4x4 ProjectionMatrix;
    Vector3 CameraPosition;
    float Time = 0.0f;
};

__declspec(align(16))
struct MeshConstantBuffer
{
    Matrix4x4 LocalMatrix;
};

struct VertexDataScreen
{
    Vector3 Position;
};

struct VertexWorld
{
    Vector3 Position;
    Vector3 Color;
    Vector3 Color2;
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
    Matrix4x4 LocalMatrix() { return m_localMatrix; }

protected:
    Shader* m_shader;
    unsigned long m_verticesCount;
    unsigned long m_indexCount;
    DrawcallType m_type;
    Matrix4x4 m_localMatrix;
};

}
