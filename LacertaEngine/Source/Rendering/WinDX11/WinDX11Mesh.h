#pragma once
#include <d3d11.h>
#include "../../Maths/Maths.h"
#include "../Mesh.h"

namespace LacertaEngine
{

struct WinDX11ShapeData
{
    ID3D11Buffer* Vbo;
    unsigned long VerticesSize;
    ID3D11Buffer* Ibo;
    unsigned long IndexesSize;
};
    
class LACERTAENGINE_API WinDX11Mesh : public Mesh
{
public:
    WinDX11Mesh();
    ~WinDX11Mesh();

    void CreateResource(const wchar_t* filePath, Renderer* renderer) override;
    const std::vector<WinDX11ShapeData>& GetWinDX11ShapesData() const { return m_shapesData; }

    // TODO remove, transition method
    const std::vector<ShapeData> GetShapesData() const override;

private:
    void ComputeTangents(const Vector3& v0, const Vector3& v1, const Vector3& v2,
        const Vector2 t0, const Vector2 t1, const Vector2 t2,
        Vector3& tangent, Vector3& binormal);
    
    std::vector<WinDX11ShapeData> m_shapesData;
};

}

