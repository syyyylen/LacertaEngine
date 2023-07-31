#include "EditorWindow.h"

#include "../ImGui/imgui_impl_win32.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace LacertaEngineEditor
{
    
EditorWindow::EditorWindow(LPCWSTR windowName): Window(windowName)
{
}

EditorWindow::~EditorWindow()
{
}

void EditorWindow::OnCreate()
{
    Window::OnCreate();
}

void EditorWindow::OnUpdate()
{
    Window::OnUpdate();
}

void EditorWindow::OnDestroy()
{
    Window::OnDestroy();
}

void EditorWindow::OnFocus()
{
    Window::OnFocus();
}

void EditorWindow::OnLooseFocus()
{
    Window::OnLooseFocus();
}

void EditorWindow::OnResize()
{
    Window::OnResize();

    RECT windowRect = GetClientWindowRect();
    GraphicsEngine::Get()->Resize(windowRect.right, windowRect.bottom);
}

bool EditorWindow::OnWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}
    
}

