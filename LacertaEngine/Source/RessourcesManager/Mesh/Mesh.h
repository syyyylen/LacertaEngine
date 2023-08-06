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

    void SetBuffersData(void* vbo, void* ibo)
    {
        m_vbo = vbo;
        m_ibo = ibo;
    }

    void* GetVBO() { return m_vbo; }
    unsigned long GetVerticesSize() { return m_verticesSize; }
    void* GetIBO() { return m_ibo; }
    unsigned long GetIndicesSize() { return m_indexesSize; }

private:
    void* m_vbo;
    unsigned long m_verticesSize;
    void* m_ibo;
    unsigned long m_indexesSize;
};
    
}
