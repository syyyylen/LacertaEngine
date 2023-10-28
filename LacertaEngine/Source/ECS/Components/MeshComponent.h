#pragma once
#include "../Component.h"
#include "../../Rendering/Material.h"
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

    void SetMaterial(Material* material) { m_material = material; }
    Material* GetMaterial() { return m_material; }
    
private:
    Mesh* m_mesh;
    Material* m_material;
};
    
}
