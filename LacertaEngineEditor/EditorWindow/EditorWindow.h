#pragma once
#include <LacertaEngine.h>

namespace LacertaEngineEditor
{
    using namespace LacertaEngine; // This is making the code less explicit. Might remove it if it's too confusing. 

/**
 * \brief Handle the main Editor Window that will contain the scene viewport, ImGui menus etc...
 */
class EditorWindow : public Window
{
public:
    EditorWindow(LPCWSTR windowName);
    ~EditorWindow();

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

