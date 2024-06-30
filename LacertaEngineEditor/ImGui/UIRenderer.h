#pragma once

#include "UIPanel.h"
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

    void InitializeUI(HWND hwnd, LacertaEditor* editor, RendererType rendererType);
    void Update();
    LacertaEditor* GetEditor() { return m_editor; }

private:
    inline static RendererType m_rendererType;
    std::vector<UIPanel*> m_panels;
    static UIRenderer* s_UIRenderer;
    LacertaEditor* m_editor;
};
    
}
