#pragma once
#include <LacertaEngine.h>

#include "Window/Window.h"

namespace LacertaEngineEditor
{
    using namespace LacertaEngine; // This is making the code less explicit. Might remove it if it's too confusing. 

/**
 * \brief Handle the main Editor Window that will contain the scene viewport, ImGui menus etc...
 */
class EditorWindow : public Window, public InputListener
{
public:
    EditorWindow(LPCWSTR windowName);
    ~EditorWindow();

    // InputListener interface
    void OnKeyDown(int key) override;
    void OnKeyUp(int key) override;
    void OnMouseMove(const Vector2& mousePosition) override;
    virtual void OnLeftMouseDown(const Vector2& mousePos) override;
    virtual void OnRightMouseDown(const Vector2& mousePos) override;
    virtual void OnLeftMouseUp(const Vector2& mousePos) override;
    virtual void OnRightMouseUp(const Vector2& mousePos) override;
    // end InputListener interface

    // Window overrides
    virtual void OnCreate() override;
    virtual void OnUpdate() override;
    virtual void OnDestroy() override;
    virtual void OnFocus() override;
    virtual void OnLooseFocus() override;
    virtual void OnResize() override;
    virtual bool OnWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
    // end Window overrides
};

}

