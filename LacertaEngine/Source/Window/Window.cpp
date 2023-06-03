#include "Window.h"

#include "../Logger/Logger.h"

namespace LacertaEngine
{
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        switch(msg)
        {
        case WM_CREATE:
            {
                LOG(Debug, "Window Create");
                break;
            }

        case WM_SIZE:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(window)
                    LOG(Debug, "Window Resize");
                break;
            }

        case WM_SETFOCUS:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(window)
                    LOG(Debug, "Window Set Focus");
                break;
            }

        case WM_KILLFOCUS:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(window)
                    LOG(Debug, "Window Kill Focus");
                break;
            }

        case WM_DESTROY:
            {
                Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
                if(window)
                    LOG(Debug, "Window Destroy");
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
}
