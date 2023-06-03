﻿#pragma once

#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Window
{
public:
    Window(LPCWSTR windowName);
    ~Window();

    bool IsRunning();

    // Window interface
    virtual void OnCreate();
    virtual void OnUpdate();
    virtual void OnDestroy();
    virtual void OnFocus();
    virtual void OnLooseFocus();
    virtual void OnResize();

    // Utilites
    bool IsMinimized() const;
    bool IsActive() const;

    void SetPosition(UINT x, UINT y);
    void Show();
    void Hide();
    void Destroy();
    void SetMaximized(bool maximized);
    
    RECT GetClientWindowRect();
    RECT GetScreenSize();

private:
    bool Broadcast();

protected:
    bool m_isRunning;
    HWND m_hwnd;
    HINSTANCE m_hinstance;
    bool m_maximized;
    RECT m_unmaxedRect;
};
    
}
