#pragma once
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
    
class LACERTAENGINE_API Mesh : public GraphicsResource
{
public:
    Mesh();
    ~Mesh();

    virtual void CreateResource(const wchar_t* filePath, Renderer* renderer) = 0;

    // TODO remove, transition method
    virtual const std::vector<ShapeData> GetShapesData() const = 0;
};

}

