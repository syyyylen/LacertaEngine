#pragma once

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
    virtual bool OnWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Override to add ImGui proc handler

    // Utilites
    bool IsMinimized() const;
    bool IsActive() const;

    void SetPosition(UINT x, UINT y);
    void Show();
    void Hide();
    void Destroy();
    void SetFullscreen(bool fullscreen);
    void Maximize();

    HWND GetHWND();
    RECT GetClientWindowRect();
    RECT GetScreenSize();

private:
    bool Broadcast();

protected:
    bool m_isRunning = false;
    HWND m_hwnd;
    HINSTANCE m_hinstance;
    bool m_fullscreen = false;
    RECT m_unmaxedRect;

    bool m_isInit = false; // TODO remove this and use evenmential approach
};
    
}
