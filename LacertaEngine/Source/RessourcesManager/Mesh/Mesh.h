#pragma once
#include "../Resource.h"
#include "../../Rendering/Drawcall.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Mesh : public Resource
{
public:
    Mesh();
    ~Mesh();
    
    void CreateResource(const wchar_t* filePath) override;

    std::vector<VertexMesh> GetVertices() { return m_verticesList; }
    std::vector<unsigned int> GetIndices() { return m_indicesList; }

private:
    std::vector<VertexMesh> m_verticesList;
    std::vector<unsigned int> m_indicesList;
};
    
}
