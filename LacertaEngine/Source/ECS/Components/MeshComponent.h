#pragma once
#include "../Component.h"
#include "../../RessourcesManager/Mesh/Mesh.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API MeshComponent : public Component
{
public:
    MeshComponent();
    ~MeshComponent();

    void SetMesh(Mesh* mesh) { m_mesh = mesh; }
    Mesh* GetMesh() { return m_mesh; }

private:
    Mesh* m_mesh;

public:
    // TODO add a Material class that stores Shader / Data (which differs in every mat instance)
    const wchar_t* VertexShaderPath;
    const wchar_t* PixelShaderPath;
};
    
}
