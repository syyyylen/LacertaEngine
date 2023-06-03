#include "EditorWindow.h"

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

void EditorWindow::OnMouseMove(const Point& mousePos)
{
}

void EditorWindow::OnLeftMouseDown(const Point& mousePos)
{
}

void EditorWindow::OnRightMouseDown(const Point& mousePos)
{
}

void EditorWindow::OnLeftMouseUp(const Point& mousePos)
{
}

void EditorWindow::OnRightMouseUp(const Point& mousePos)
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
}

}

