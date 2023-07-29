#include "LacertaEditor.h"

#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_src/imgui.h"
#include "Rendering/WinDX11/WinDX11Renderer.h"
#include "Rendering/WinDX11/WinDX11RenderTarget.h"

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

    // ----------------------- Window Creation ------------------------

    m_editorWindow = new EditorWindow(L"Lacerta Engine");
    // m_editorWindow->Maximize();

    // ----------------------- Input System Creation ------------------------

    InputSystem::Create();

    // ----------------------- Graphics Engine Creation & Renderer Initialization  ------------------------
    
    GraphicsEngine::Create();
    
    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    HWND hwnd = m_editorWindow->GetHWND();
    
    GraphicsEngine::Get()->InitializeRenderer((int*)hwnd, RendererType::RENDERER_WIN_DX11, width, height, 24, 60);

    // ----------------------------- UI Initialization  ------------------------

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)GraphicsEngine::Get()->GetRenderer(); // TODO remove direct reference to DX11
    ImGui_ImplDX11_Init((ID3D11Device*)Dx11Renderer->GetDriver(), Dx11Renderer->GetImmediateContext());
}

void LacertaEditor::Update()
{
    InputSystem::Get()->Update();

    // ----------------------------- Rendering Update  --------------------------

    WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)GraphicsEngine::Get()->GetRenderer(); // TODO remove direct reference to DX11
    WinDX11RenderTarget* Dx11RenderTarget = (WinDX11RenderTarget*)Dx11Renderer->GetRenderTarget();
    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    Dx11RenderTarget->SetViewportSize(Dx11Renderer, width, height);

    GraphicsEngine::Get()->Render();

    // ----------------------------- UI Update  --------------------------

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    { // My imgui test window
        ImGui::Begin("FrameRate");                  
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // ------------------------ UI & DrawCalls done, present Swap Chain now ----------------
    
    GraphicsEngine::Get()->PresentSwapChain();
}

void LacertaEditor::Quit()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    if(m_editorWindow)
        m_editorWindow->Destroy();

    GraphicsEngine::Shutdown();
    
    InputSystem::Release();

    LOG(Debug, "Lacerta Editor : Quit");
    
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
