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
    void ComputeTangents(const Vector3& v0, const Vector3& v1, const Vector3& v2,
        const Vector2 t0, const Vector2 t1, const Vector2 t2,
        Vector3& tangent, Vector3& binormal);
    
    std::vector<ShapeData> m_shapesData;
};
    
}
