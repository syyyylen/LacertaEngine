#pragma once
#include "Drawable.h"
#include "GraphicsResource.h"

namespace LacertaEngine
{

// TODO remove me
struct ShapeData
{
    void* Vbo;
    unsigned long VerticesSize;
    void* Ibo;
    unsigned long IndexesSize;
};

class LACERTAENGINE_API Shape : public Drawable
{
public:
    Shape();
    ~Shape();

    virtual void BindBuffers(Renderer* renderer) = 0;
    virtual unsigned long GetVerticesSize() = 0;
    virtual unsigned long GetIndicesSize() = 0;
};

class LACERTAENGINE_API Mesh : public GraphicsResource
{
public:
    Mesh();
    ~Mesh();

    virtual void CreateResource(const wchar_t* filePath, Renderer* renderer) = 0;
    virtual const std::vector<Shape*> GetShapesData() { return m_shapesData; }

protected:
    std::vector<Shape*> m_shapesData;
};

}

