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

void EditorWindow::OnKeyDown(int key)
{
}

void EditorWindow::OnKeyUp(int key)
{
}

void EditorWindow::OnMouseMove(const Vector2& mousePos)
{
    return;
    
    int width = GetClientWindowRect().right - GetClientWindowRect().left;
    int height = GetClientWindowRect().bottom - GetClientWindowRect().top;

    InputSystem::Get()->SetCursorPosition(Vector2(width/2.0f, height/2.0f));
}

void EditorWindow::OnLeftMouseDown(const Vector2& mousePos)
{
}

void EditorWindow::OnRightMouseDown(const Vector2& mousePos)
{
}

void EditorWindow::OnLeftMouseUp(const Vector2& mousePos)
{
}

void EditorWindow::OnRightMouseUp(const Vector2& mousePos)
{
}

void EditorWindow::OnCreate()
{
    Window::OnCreate();
    InputSystem::Get()->AddListener(this);
}

void EditorWindow::OnUpdate()
{
    Window::OnUpdate();
}

void EditorWindow::OnDestroy()
{
    Window::OnDestroy();
    InputSystem::Get()->RemoveListener(this);
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

