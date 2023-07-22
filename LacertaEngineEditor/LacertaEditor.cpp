#include "LacertaEditor.h"

namespace LacertaEngineEditor
{

LacertaEditor::LacertaEditor()
{
}

LacertaEditor::~LacertaEditor()
{
}

void LacertaEditor::Start()
{
    LOG(Debug, "Lacerta Editor : Start");

    m_editorWindow = new EditorWindow(L"EngineWindow");
    m_editorWindow->Maximize();

    InputSystem::Create();

    m_graphicsEngine = new GraphicsEngine();
    
    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    HWND hwnd = m_editorWindow->GetHWND();
    
    m_graphicsEngine->InitializeRenderer((int*)hwnd, RendererType::RENDERER_WIN_DX11, width, height, 24, 60);
}

void LacertaEditor::Update()
{
    InputSystem::Get()->Update();

    // TODO update the Scene

    if(m_graphicsEngine)
    {
        m_graphicsEngine->Render();
    }
}

void LacertaEditor::Quit()
{
    if(m_editorWindow)
        m_editorWindow->Destroy();

    if(m_graphicsEngine)
    {
        m_graphicsEngine->Shutdown();
        delete m_graphicsEngine;
    }
    
    InputSystem::Release();
    
    Logger::Get()->WriteLogsToFile();

    LOG(Debug, "Lacerta Editor : Quit");
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
