#pragma once
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Resource
{
public:
    Resource();
    ~Resource();

    virtual void CreateResource(const wchar_t* filePath) = 0;
    void SetFilePath(const wchar_t* filePath);
    const wchar_t* GetFilePath() { return m_filePath; }
    
private:
    const wchar_t* m_filePath;
};
    
}
