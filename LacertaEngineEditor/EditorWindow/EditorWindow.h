#pragma once
#include "InputSystem/InputListener.h"
#include "Window/Window.h"
#include "Core.h"

namespace LacertaEngineEditor
{

using namespace LacertaEngine; // This is making the code less explicit. Might remove it if it's too confusing. 

class EditorWindow : public Window, public InputListener
{
public:
    EditorWindow(LPCWSTR windowName);
    ~EditorWindow();

    // InputListener interface
    void OnKeyDown(int key) override;
    void OnKeyUp(int key) override;
    void OnMouseMove(const Point& mousePosition) override;
    virtual void OnLeftMouseDown(const Point& mousePos) override;
    virtual void OnRightMouseDown(const Point& mousePos) override;
    virtual void OnLeftMouseUp(const Point& mousePos) override;
    virtual void OnRightMouseUp(const Point& mousePos) override;
    // end InputListener interface

    // Window overrides
    virtual void OnCreate() override;
    virtual void OnUpdate() override;
    virtual void OnDestroy() override;
    virtual void OnFocus() override;
    virtual void OnLooseFocus() override;
    virtual void OnResize() override;
    // end Window overrides
    
};

}

