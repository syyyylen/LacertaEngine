#pragma once
#include "../Resource.h"
#include "../../Rendering/Drawcall.h"

namespace LacertaEngine
{

struct ShapeData
{
    void* Vbo;
    unsigned long VerticesSize;
    void* Ibo;
    unsigned long IndexesSize;
};
    
class LACERTAENGINE_API Mesh : public Resource
{
public:
    Mesh();
    ~Mesh();
    
    void CreateResource(const wchar_t* filePath) override;

    const std::vector<ShapeData>& GetShapesData() const { return m_shapesData; }

private:
    std::vector<ShapeData> m_shapesData;
};
    
}
