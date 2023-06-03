#include "Window.h"

#include "../Utilities.h"
#include "../Logger/Logger.h"

namespace LacertaEngine
{
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch(msg)
        {
        case WM_CREATE:
            {
                LPCREATESTRUCT pCreateStruct = (LPCREATESTRUCT)lparam;
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pCreateStruct->lpCreateParams);
                break;
            }

        case WM_SIZE:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(window)
                    window->OnResize();
                break;
            }

        case WM_SETFOCUS:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(window)
                    window->OnFocus();
                break;
            }

        case WM_KILLFOCUS:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(window)
                    window->OnLooseFocus();
                break;
            }

        case WM_DESTROY:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(window)
                    window->OnDestroy();
                ::PostQuitMessage(0);
                break;
            }

        default:
            return ::DefWindowProcW(hwnd, msg, wparam, lparam);
        }

        return NULL;
    }
    
    Window::Window(LPCWSTR windowName)
    {
        LOG(Debug, "Creating a window");

        // Window properties
        WNDCLASSEXW wc;
        wc.cbClsExtra = NULL;
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.cbWndExtra = NULL;
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
        wc.hInstance = NULL;
        wc.lpszClassName = L"LacertaEngineBaseWindowClass";
        wc.lpszMenuName = L"";
        wc.style = NULL;
        wc.lpfnWndProc = &WndProc;

        if(!::RegisterClassExW(&wc))
            LOG(Error, "Window class not created");

        // Creation of the window 
        m_hwnd = ::CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, L"LacertaEngineBaseWindowClass", windowName, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, NULL, this);

        if(!m_hwnd)
            LOG(Error, "Window not created");

        // Show up the window
        ::ShowWindow(m_hwnd, SW_SHOW);
        ::UpdateWindow(m_hwnd);

        // Set this flag to true to indicate that the window is running
        m_isRunning = true;
    }

    Window::~Window()
    {
        ::DestroyWindow(m_hwnd);
    }

    bool Window::IsRunning()
    {
        if(m_isRunning)
            Broadcast();
        
        return m_isRunning;
    }

    void Window::OnCreate()
    {
        LOG(Debug, "Window OnCreate!");
    }

    void Window::OnUpdate()
    {
        // LOG(Debug, "Window OnUpdate!");
    }

    void Window::OnDestroy()
    {
        LOG(Debug, "Window OnDestroy!");
        
        m_isRunning = false;
    }

    void Window::OnFocus()
    {
        LOG(Debug, "Window OnFocus!");
    }

    void Window::OnLooseFocus()
    {
        LOG(Debug, "Window OnLooseFocus!");
    }

    void Window::OnResize()
    {
        LOG(Debug, "Window OnResize!");
    }

    bool Window::IsMinimized() const
    {
        return IsIconic(m_hwnd) != 0;
    }

    bool Window::IsActive() const
    {
        return GetForegroundWindow() == m_hwnd;
    }

    void Window::SetPosition(UINT x, UINT y)
    {
        HR(SetWindowPos(m_hwnd, HWND_NOTOPMOST, x, y, 0, 0, SWP_NOSIZE));
    }

    void Window::Show()
    {
        ShowWindow(m_hwnd, SW_SHOW);
    }

    void Window::Hide()
    {
        ShowWindow(m_hwnd, SW_HIDE);
    }

    void Window::Destroy()
    {
        HR(DestroyWindow(m_hwnd));
    }

    void Window::SetMaximized(bool maximized)
    {
        if (maximized != m_maximized)
        {
            m_maximized = maximized;

            // TODO handle this differently
            DWORD style = GetWindowLong(m_hwnd, GWL_STYLE);
            DWORD extendedStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);
            
            if (m_maximized)
            {
                // Create the new style that has no menu bars or borders
                DWORD newStyle = style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
                DWORD newExStyle = extendedStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

                // Save information so that we can be un-maximized
                GetWindowRect(m_hwnd, &m_unmaxedRect);

                // Determine the monitor we are on
                HMONITOR curMonitor = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTOPRIMARY);

                MONITORINFO monitorInfo;
                monitorInfo.cbSize = sizeof(MONITORINFO);

                GetMonitorInfo(curMonitor, &monitorInfo);

                // Determine the position to put it at
                UINT x = monitorInfo.rcMonitor.left;
                UINT y = monitorInfo.rcMonitor.top;
                UINT width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
                UINT height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

                // place the window
                HR(SetWindowLong(m_hwnd, GWL_STYLE, newStyle));
                HR(SetWindowLong(m_hwnd, GWL_EXSTYLE, newExStyle));
                HR(SetWindowPos(m_hwnd, HWND_TOP, x, y, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW));
            }
            else
            {
                // Determine the location to put the window back to
                UINT x = m_unmaxedRect.left;
                UINT y = m_unmaxedRect.top;
                UINT width = m_unmaxedRect.right - m_unmaxedRect.left;
                UINT height = m_unmaxedRect.bottom - m_unmaxedRect.top;

                HR(SetWindowLong(m_hwnd, GWL_STYLE, style));
                HR(SetWindowLong(m_hwnd, GWL_EXSTYLE, extendedStyle));
                HR(SetWindowPos(m_hwnd, HWND_NOTOPMOST, x, y, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW));
            }
        }
    }


    RECT Window::GetClientWindowRect()
    {
        RECT rc;
        ::GetClientRect(m_hwnd, &rc);
        return rc;
    }

    RECT Window::GetScreenSize()
    {
        RECT rc;
        rc.right = ::GetSystemMetrics(SM_CXSCREEN);
        rc.bottom = ::GetSystemMetrics(SM_CYSCREEN);
        return rc;
    }

    bool Window::Broadcast()
    {
        MSG msg;
        while(::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) > 0)
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
        }

        OnUpdate();

        return true;
    }
}
