#include "LacertaEditor.h"

#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_src/imgui.h"
#include "Rendering/Drawcall.h"
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

    // ----------------------- Starting Global Timer -----------------

    m_globalTimer = new Timer();

    // ----------------------- Window Creation ------------------------

    m_editorWindow = new EditorWindow(L"Lacerta Engine");

    // ----------------------- Input System Creation ------------------------

    InputSystem::Create();

    // ----------------------- Graphics Engine Creation & Renderer Initialization  ------------------------
    
    GraphicsEngine::Create();
    
    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    HWND hwnd = m_editorWindow->GetHWND();
    
    GraphicsEngine::Get()->InitializeRenderer((int*)hwnd, RendererType::RENDERER_WIN_DX11, width, height, 24, 60);

    // ------------------------------- Debug Drawcall ------------------------------

    VertexDataScreen screenVertices[] =
    {
        { Vector3(-0.5f, -0.5f, 0.0f) },
        { Vector3(0.0f, 0.65f, 0.0f) },
        { Vector3(0.5f, -0.5f, 0.0f) }
    };
    
    DrawcallData dcData = {};
    dcData.Data = &screenVertices;
    dcData.Size = ARRAYSIZE(screenVertices);
    dcData.Type = DrawcallType::Screen;
    dcData.VertexShaderPath = L"../LacertaEngine/Source/Rendering/Shaders/ScreenVertex.hlsl";
    dcData.PixelShaderPath = L"../LacertaEngine/Source/Rendering/Shaders/SimpleColorPixelShader.hlsl";
    
    GraphicsEngine::Get()->AddDrawcall(&dcData);
    

    // ------------------------------- Debug Cube ------------------------------

    // VertexWorld screenVertices[] = 
    // {
    //     //X - Y - Z
    //     //FRONT FACE
    //     {Vector3(-0.5f,-0.5f,-0.5f),    Vector3(1,0,0),  Vector3(0.2f,0,0) },
    //     {Vector3(-0.5f,0.5f,-0.5f),    Vector3(1,1,0), Vector3(0.2f,0.2f,0) },
    //     { Vector3(0.5f,0.5f,-0.5f),   Vector3(1,1,0),  Vector3(0.2f,0.2f,0) },
    //     { Vector3(0.5f,-0.5f,-0.5f),     Vector3(1,0,0), Vector3(0.2f,0,0) },
    //
    //     //BACK FACE
    //     { Vector3(0.5f,-0.5f,0.5f),    Vector3(0,1,0), Vector3(0,0.2f,0) },
    //     { Vector3(0.5f,0.5f,0.5f),    Vector3(0,1,1), Vector3(0,0.2f,0.2f) },
    //     { Vector3(-0.5f,0.5f,0.5f),   Vector3(0,1,1),  Vector3(0,0.2f,0.2f) },
    //     { Vector3(-0.5f,-0.5f,0.5f),     Vector3(0,1,0), Vector3(0,0.2f,0) }
    //
    // };
    //
    // unsigned int indexList[]=
    // {
    //     //FRONT SIDE
    //     0,1,2,  //FIRST TRIANGLE
    //     2,3,0,  //SECOND TRIANGLE
    //     //BACK SIDE
    //     4,5,6,
    //     6,7,4,
    //     //TOP SIDE
    //     1,6,5,
    //     5,2,1,
    //     //BOTTOM SIDE
    //     7,0,3,
    //     3,4,7,
    //     //RIGHT SIDE
    //     3,2,5,
    //     5,4,3,
    //     //LEFT SIDE
    //     7,6,1,
    //     1,0,7
    // };
    //
    // DrawcallData dcData = {};
    // dcData.Data = &screenVertices;
    // dcData.Size = ARRAYSIZE(screenVertices);
    // dcData.Type = DrawcallType::Mesh;
    // dcData.VertexShaderPath = L"../LacertaEngine/Source/Rendering/Shaders/WorldVertex.hlsl";
    // dcData.PixelShaderPath = L"../LacertaEngine/Source/Rendering/Shaders/SimpleColorPixelShader.hlsl";
    // dcData.IndexesData = &indexList;
    // dcData.IndexesSize = ARRAYSIZE(indexList);
    //
    // GraphicsEngine::Get()->AddDrawcall(&dcData);

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

    m_editorWindow->Maximize();
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

    ConstantBuffer cc;
    cc.Time = m_globalTimer->Elapsed();
    GraphicsEngine::Get()->UpdateShaderConstants(&cc);

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
