#pragma once
#include <d3d11.h>
#include "../../Maths/Maths.h"
#include "../Mesh.h"

namespace LacertaEngine
{

class LACERTAENGINE_API WinDX11Shape : public Shape
{
public:
    WinDX11Shape(ID3D11Buffer* vbo, unsigned long verticesSize, ID3D11Buffer* ibo, unsigned long indexesSize);
    ~WinDX11Shape() override;

    void BindBuffers(Renderer* renderer) override;
    unsigned long GetVerticesSize() override { return m_verticesSize; }
    unsigned long GetIndicesSize() override { return m_indexesSize; }

private:
    ID3D11Buffer* m_vbo;
    unsigned long m_verticesSize;
    ID3D11Buffer* m_ibo;
    unsigned long m_indexesSize;
};
    
class LACERTAENGINE_API WinDX11Mesh : public Mesh
{
public:
    WinDX11Mesh();
    ~WinDX11Mesh() override;

    void CreateResource(const wchar_t* filePath, Renderer* renderer) override;

private:
    void ComputeTangents(const Vector3& v0, const Vector3& v1, const Vector3& v2,
        const Vector2 t0, const Vector2 t1, const Vector2 t2,
        Vector3& tangent, Vector3& binormal);
};

}

