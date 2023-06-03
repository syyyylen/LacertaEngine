#include "LacertaEditor.h"

namespace LacertaEngineEditor
{

LacertaEditor::LacertaEditor()
{
    m_editorWindow = new EditorWindow(L"EngineWindow");
    InputSystem::Create();
}

LacertaEditor::~LacertaEditor()
{
}

void LacertaEditor::Start()
{
    LOG(Debug, "Lacerta Editor : Start");
}

void LacertaEditor::Update()
{
    InputSystem::Get()->Update();
}

void LacertaEditor::Quit()
{
    LOG(Debug, "Lacerta Editor : Quit");
    
    if(m_editorWindow)
        m_editorWindow->Destroy();

    InputSystem::Release();
    Logger::Get()->WriteLogsToFile();
}

bool LacertaEditor::IsRunning()
{
    return m_editorWindow->IsRunning();
}

EditorWindow* LacertaEditor::GetEditorWindow()
{
    return m_editorWindow;
}
    
}
