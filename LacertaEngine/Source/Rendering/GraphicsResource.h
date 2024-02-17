#pragma once
#include "../Core.h"

namespace LacertaEngine
{

class Renderer;
    
class LACERTAENGINE_API GraphicsResource
{
public:
    GraphicsResource();
    ~GraphicsResource();

    virtual void CreateResource(const wchar_t* filePath, Renderer* renderer) = 0;
    void SetFilePath(const wchar_t* filePath);
    const wchar_t* GetFilePath() { return m_filePath; }
    
private:
    const wchar_t* m_filePath;
};

}

