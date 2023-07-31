#pragma once
#include "../Resource.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Mesh : public Resource
{
public:
    Mesh();
    ~Mesh();

    void CreateResource(const wchar_t* filePath) override;

    std::string GetSomeString() { return m_someString; }

private:
    std::string m_someString;
};
    
}
