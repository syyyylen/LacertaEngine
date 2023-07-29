#pragma once
#include "../Core.h"
#include "Renderer.h"
#include "../Maths/Vector3.h"

namespace LacertaEngine
{

struct VertexDataScreen
{
    Vector3 Position;
};

class Shader;


class LACERTAENGINE_API Drawcall
{
public:
    Drawcall();
    virtual ~Drawcall();

    virtual void Setup(Renderer* renderer) = 0;
    virtual void Pass(Renderer* renderer) = 0;
    virtual void CreateVBO(Renderer* renderer, void* data, unsigned long size) = 0;

    virtual void* GetVBO() = 0;
    unsigned long GetVerticesCount() { return m_verticesCount; }

protected:
    Shader* m_shader;
    unsigned long m_verticesCount;
};

}
