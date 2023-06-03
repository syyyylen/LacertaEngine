#pragma once

#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Window
{
public:
    Window(LPCWSTR windowName);
    ~Window();

protected:
    bool m_isRunning;
    HWND m_hwnd;
};
    
}
