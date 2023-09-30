#pragma once

#include "../LacertaEditor.h"
#include "../EditorWindow/EditorWindow.h"

namespace LacertaEngineEditor
{
    
class UIRenderer
{
private:
    UIRenderer();
    ~UIRenderer();

public:
    static UIRenderer* Get();
    static void Create();
    static void Shutdown();

    void InitializeUI(HWND hwnd, LacertaEditor* editor);
    void Update();

private:
    static UIRenderer* s_UIRenderer;
    LacertaEditor* m_editor;
};
    
}
