#pragma once
#include "Drawable.h"
#include "GraphicsResource.h"

namespace LacertaEngine
{

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
    virtual ~Mesh();

    virtual void CreateResource(const wchar_t* filePath, Renderer* renderer) = 0;
    virtual void ImportMesh(std::string filePath, Renderer* renderer) = 0;
    virtual const std::vector<Shape*> GetShapesData() { return m_shapesData; }

protected:
    std::vector<Shape*> m_shapesData;
};

}

