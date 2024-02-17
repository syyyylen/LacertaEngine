#include "GraphicsResource.h"

namespace LacertaEngine
{
    
GraphicsResource::GraphicsResource()
{
}

GraphicsResource::~GraphicsResource()
{
}

void GraphicsResource::SetFilePath(const wchar_t* filePath)
{
    m_filePath = filePath;
}
    
}
