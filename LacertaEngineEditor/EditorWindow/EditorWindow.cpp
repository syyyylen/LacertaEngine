#include "EditorWindow.h"

#include "Logger/Logger.h"

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
    LOG(Debug, "OnKeyDown!");
}

void EditorWindow::OnKeyUp(int key)
{
}

void EditorWindow::OnMouseMove(const Point& mousePosition)
{
}

void EditorWindow::OnLeftMouseDown(const Point& mousePos)
{
    LOG(Debug, "Left Mouse button down at pos :" + mousePos.ToString());
}

void EditorWindow::OnRightMouseDown(const Point& mousePos)
{
    LOG(Debug, "Right Mouse button down at pos :" + mousePos.ToString());
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
}

void EditorWindow::OnUpdate()
{
    Window::OnUpdate();
}

void EditorWindow::OnDestroy()
{
    Window::OnDestroy();

    LOG(Debug, "Editor Window destroyed !");
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

