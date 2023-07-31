#include "Mesh.h"

namespace LacertaEngine
{
    
Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::CreateResource(const wchar_t* filePath)
{
    m_someString = "Seluj reinnav";
    SetFilePath(filePath);
}
    
}
