#include "LacertaEditor.h"

#include <execution>

#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_src/imgui.h"
#include "Rendering/Drawcall.h"
#include "Rendering/WinDX11/WinDX11Renderer.h"
#include "Rendering/WinDX11/WinDX11RenderTarget.h"

namespace LacertaEngineEditor
{

static float s_mouseSensivity = 3.0f;
static float s_moveSpeed = 2.5f;
static float s_strafeMoveSpeed = 2.5f;
static float s_inputDownScalar = 0.03f;

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

    // ----------------------- Asset Mgr Creation ------------------------

    ResourceManager::Create();

    // ----------------------- Window Creation ------------------------

    m_editorWindow = new EditorWindow(L"Lacerta Engine");

    // ----------------------- Input System Creation ------------------------

    InputSystem::Create();
    InputSystem::Get()->AddListener(this);
    InputSystem::Get()->ShowCursor(false);
    
    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    InputSystem::Get()->SetCursorPosition(Vector2(width/2.0f, height/2.0f));

    // ----------------------- Graphics Engine Creation & Renderer Initialization  ------------------------
    
    GraphicsEngine::Create();
    HWND hwnd = m_editorWindow->GetHWND();
    GraphicsEngine::Get()->InitializeRenderer((int*)hwnd, RendererType::RENDERER_WIN_DX11, width, height, 24, 60);

    // ----------------------------- Debug GO Creation -----------------------
    
    Mesh* teaPotMesh = ResourceManager::Get()->CreateResource<Mesh>(L"Assets/Meshes/teapot.obj");

    float rdmDist = 20.0f;
    float maxScaleMult = 4.0f;
    for(int i = 0; i < 50; i++)
    {
        GameObject* teapotGo = new GameObject(); // Every single scene obj heap allocated just feels bad
        MeshComponent* teapotMeshComp = new MeshComponent(); // Every single component heap allocated just feels bad

        teapotMeshComp->SetMesh(teaPotMesh);
        teapotMeshComp->VertexShaderPath = L"../LacertaEngine/Source/Rendering/Shaders/MeshVertex.hlsl"; // TODO material system
        teapotMeshComp->PixelShaderPath = L"../LacertaEngine/Source/Rendering/Shaders/MeshPixelShader.hlsl"; // TODO material system

        teapotGo->AddComponent(teapotMeshComp);

        teapotGo->SetPosition(Vector3(Random::RandomFloatRange(-rdmDist, rdmDist),
                                        Random::RandomFloatRange(-rdmDist, rdmDist),
                                        Random::RandomFloatRange(-rdmDist, rdmDist)));

        float rdmScale = Random::RandomFloatRange(0.5f, maxScaleMult);
        teapotGo->SetScale(Vector3(rdmScale, rdmScale, rdmScale));
        
        m_sceneGameObjects.push_back(teapotGo);
    }

    // ----------------------------- Debug Scene Draw Objects -----------------------

    // TODO move this into a proper Scene system, and into the Update function
    for(auto go : m_sceneGameObjects)
    {
        std::list<Component*> components = go->GetComponentsList();
        for(auto comp : components)
        {
            MeshComponent* meshComp = static_cast<MeshComponent*>(comp);
            if(meshComp)
            {
                Mesh* mesh = meshComp->GetMesh();
                if(mesh)
                {
                    std::vector<VertexMesh> meshVertData = mesh->GetVertices();
                    std::vector<unsigned int> meshIndices = mesh->GetIndices();
                    DrawcallData dcData = {};
                    dcData.Data = &meshVertData[0];
                    dcData.Size = meshVertData.size();
                    dcData.Type = DrawcallType::dcMesh;
                    dcData.VertexShaderPath = meshComp->VertexShaderPath;
                    dcData.PixelShaderPath = meshComp->PixelShaderPath;
                    dcData.IndexesData = &meshIndices[0];
                    dcData.IndexesSize = meshIndices.size();
                    dcData.LocalMatrix = go->GetTransform()->GetTransformMatrix();

                    GraphicsEngine::Get()->AddDrawcall(&dcData);
                }
            }
        }
    }
    // TODO move this into a proper Scene system, and into the Update function
    
    // --------------------------- Camera Default Position ---------------------

    m_sceneCamera.SetTranslation(Vector3(0.0f, 0.0f, -2.5f));

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

    // ----------------------------- DeltaTime Update  --------------------------
    unsigned long oldDeltaTime = m_previousTickCount;
    m_previousTickCount = GetTickCount();
    m_deltaTime = oldDeltaTime ? (m_previousTickCount - oldDeltaTime) / 1000.0f : 0;

    // ----------------------------- Rendering Update  --------------------------

    WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)GraphicsEngine::Get()->GetRenderer(); // TODO remove direct reference to DX11
    WinDX11RenderTarget* Dx11RenderTarget = (WinDX11RenderTarget*)Dx11Renderer->GetRenderTarget();
    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    Dx11RenderTarget->SetViewportSize(Dx11Renderer, width, height);

    // TODO Camera pos & rot are updated here with
    // TODO some hard coded Matrix 
    // Constant Buffer Update 
    ConstantBuffer cc;
    cc.Time = m_globalTimer->Elapsed(); 
    
    cc.WorldMatrix.SetIdentity();
    
    Matrix4x4 worldCam;
    Matrix4x4 temp;
    
    worldCam.SetIdentity();
    temp.SetIdentity();
    temp.SetRotationX(m_cameraRotationX);
    
    worldCam *= temp;
    temp.SetIdentity();
    temp.SetRotationY(m_cameraRotationY);
    worldCam *= temp;

    Vector3 newCamPos = m_sceneCamera.GetTranslation() + worldCam.GetZDirection() * (m_cameraForward * (s_moveSpeed * s_inputDownScalar));
    newCamPos = newCamPos + worldCam.GetXDirection() * (m_cameraRight * (s_strafeMoveSpeed * s_inputDownScalar));
    worldCam.SetTranslation(newCamPos);
    cc.CameraPosition = newCamPos;
    m_sceneCamera = worldCam;
    worldCam.Inverse();
    cc.ViewMatrix = worldCam;
    
    cc.ProjectionMatrix.SetPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 100.0f);
    
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

    {
        ImGui::Begin("Debugging");
        ImGui::SliderFloat("Sensivity", &s_mouseSensivity, 0.1f, 10.0f);   
        ImGui::SliderFloat("MoveSpeed", &s_moveSpeed, 0.1f, 10.0f);
        ImGui::SliderFloat("StrafeMoveSpeed", &s_strafeMoveSpeed, 0.1f, 10.0f);
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
    
    InputSystem::Get()->RemoveListener(this);
    InputSystem::Release();

    ResourceManager::Shutdown();

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

void LacertaEditor::OnKeyDown(int key)
{
    if(key == 'Z')
    {
        m_cameraForward = 1.0f;
    }
    else if(key == 'S')
    {
        m_cameraForward = -1.0f;
    }
    else if(key == 'Q')
    {
        m_cameraRight = -1.0f;
    }
    else if(key == 'D')
    {
        m_cameraRight = 1.0f;
    }
}

void LacertaEditor::OnKeyUp(int key)
{
    m_cameraForward = 0.0f;
    m_cameraRight = 0.0f;

    if(key == 'E')
    {
        m_isMouseLocked = m_isMouseLocked ? false : true;
        InputSystem::Get()->ShowCursor(!m_isMouseLocked);
    }
}

void LacertaEditor::OnMouseMove(const Vector2& mousePosition)
{
    if(!m_isMouseLocked)
        return;
    
    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    
    m_cameraRotationX += (mousePosition.Y - (height / 2.0f)) * m_deltaTime * (s_mouseSensivity * s_inputDownScalar);
    m_cameraRotationY += (mousePosition.X - (width / 2.0f)) * m_deltaTime * (s_mouseSensivity * s_inputDownScalar);

    InputSystem::Get()->SetCursorPosition(Vector2(width/2.0f, height/2.0f));
}

void LacertaEditor::OnLeftMouseDown(const Vector2& mousePos)
{
}

void LacertaEditor::OnRightMouseDown(const Vector2& mousePos)
{
}

void LacertaEditor::OnLeftMouseUp(const Vector2& mousePos)
{
}

void LacertaEditor::OnRightMouseUp(const Vector2& mousePos)
{
}
    
}
