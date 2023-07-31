#include "Resource.h"

namespace LacertaEngine
{
    
Resource::Resource()
{
}

Resource::~Resource()
{
}

void Resource::SetFilePath(const wchar_t* filePath)
{
    m_filePath = filePath;
}
    
}
