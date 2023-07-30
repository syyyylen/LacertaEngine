#pragma once
#include <LacertaEngine.h>
#include "EditorWindow/EditorWindow.h"

namespace LacertaEngineEditor
{

using namespace LacertaEngine; // This is making the code less explicit. Might remove it if it's too confusing. 

/**
 * \brief Handle core logic for the Editor, such as Update Loops for entities etc...
 * 
 */
class LacertaEditor
{
public:
    LacertaEditor();
    ~LacertaEditor();

    void Start();
    void Update();
    void Quit();

    bool IsRunning();

    EditorWindow* GetEditorWindow();

private:
    EditorWindow* m_editorWindow;
    Timer* m_globalTimer;

    Matrix4x4 m_camera;
    float m_delta;
};
    
}
