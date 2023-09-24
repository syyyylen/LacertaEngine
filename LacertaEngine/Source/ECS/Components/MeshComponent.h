#pragma once
#include "../Component.h"
#include "../../RessourcesManager/Resources.h"
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

    void SetTexture(Texture* texture) { m_texture = texture; }
    Texture* GetTexture() { return m_texture; }

private:
    Mesh* m_mesh;
    Texture* m_texture;

public:
    // TODO add a Material class that stores Shader / Data (which differs in every mat instance)
    std::string m_shaderName;
};
    
}
