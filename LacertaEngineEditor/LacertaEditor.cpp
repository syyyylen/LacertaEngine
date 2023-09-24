#include "LacertaEditor.h"

#include <execution>

#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_src/imgui.h"
#include "Rendering/Drawcall.h"
#include "Rendering/WinDX11/WinDX11Renderer.h"
#include "Rendering/WinDX11/WinDX11RenderTarget.h"
#include "RessourcesManager/Texture/DX11Texture.h"

namespace LacertaEngineEditor
{

static float s_mouseSensivity = 3.0;
static float s_moveSpeed = 6.5f;
static float s_inputDownScalar = 0.03f;
static int item_current_idx = 0; 

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

    // ---------------------------- Debug Scene Creation --------------------

    m_activeScene = new Scene();

    // ----------------------------- Debug GO Creation -----------------------

    Mesh* statueMesh = ResourceManager::Get()->CreateResource<Mesh>(L"Assets/Meshes/statue.obj");
    Mesh* teaPotMesh = ResourceManager::Get()->CreateResource<Mesh>(L"Assets/Meshes/teapot.obj");
    Texture* sandTexture = ResourceManager::Get()->CreateTexture(L"Assets/Textures/sand.jpg"); 
    Texture* brickTexture = ResourceManager::Get()->CreateTexture(L"Assets/Textures/brick.png");

    float rdmDist = 10.0f;
    Vector3 offset = Vector3(20.0f, 0.0f, 0.0f);
    for(int i = 0; i < 60; i++)
    {
        std::string name;
        i % 2 == 0 ? name = "Teapot" : name = "Statue";
        GameObject* teapotGo = m_activeScene->CreateGameObject(name, offset + Vector3(Random::RandomFloatRange(-rdmDist, rdmDist),
                                                                                            Random::RandomFloatRange(-rdmDist, rdmDist),
                                                                                            Random::RandomFloatRange(-rdmDist, rdmDist)));
        
        MeshComponent& meshComp = teapotGo->AddComponent<MeshComponent>();
        i % 2 == 0 ? meshComp.SetMesh(teaPotMesh) : meshComp.SetMesh(statueMesh);
        i % 2 == 0 ? meshComp.SetTexture(brickTexture) : meshComp.SetTexture(sandTexture);
        meshComp.m_shaderName = "MeshShader";
    }

    // --------------------------- Camera Default Position ---------------------

    m_sceneCamera.SetTranslation(Vector3(0.0f, 0.0f, -2.5f));

    // ----------------------------- UI Initialization  ------------------------

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
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

    // ----------------------------- Debug Scene Draw Objects -----------------------

    GraphicsEngine::Get()->ClearDrawcalls();

    auto tfMeshesGroup = m_activeScene->m_registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto go : tfMeshesGroup)
    {
        auto[transform, meshComponent] = tfMeshesGroup.get<TransformComponent, MeshComponent>(go);

        // Adding DC
        Mesh* mesh = meshComponent.GetMesh();
        DrawcallData dcData = {};
        dcData.VBO = mesh->GetVBO();
        dcData.VerticesCount = mesh->GetVerticesSize();
        dcData.IBO = mesh->GetIBO();
        dcData.IndicesCount = mesh->GetIndicesSize();
        dcData.Type = DrawcallType::dcMesh;
        dcData.ShaderName = meshComponent.m_shaderName;
        dcData.LocalMatrix = transform.GetTransformMatrix();
        dcData.Texture = meshComponent.GetTexture();

        GraphicsEngine::Get()->AddDrawcall(&dcData);
    }

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
    newCamPos = newCamPos + worldCam.GetXDirection() * (m_cameraRight * (s_moveSpeed * s_inputDownScalar));
    worldCam.SetTranslation(newCamPos);
    cc.CameraPosition = newCamPos;
    m_sceneCamera = worldCam;
    worldCam.Inverse();
    cc.ViewMatrix = worldCam;
    
    cc.ProjectionMatrix.SetPerspectiveFovLH(1.57f, ((float)width / (float)height), 0.1f, 1000.0f);
    
    GraphicsEngine::Get()->UpdateShaderConstants(&cc);

    GraphicsEngine::Get()->Render();

    // ----------------------------- UI Update  --------------------------

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Render the scene texture here
    // {
    //     ImGui::Begin("DirectX11 Texture Test");
    //     ImGui::Image((void*)Dx11RenderTarget->m_sceneTextureView, ImVec2(256, 256));
    //     ImGui::End();
    // }
    
    // Dockspace wip
    static bool dockspaceOpen = false; // TODO make a viewport ImGui window and render scene as texture inside it
    if(dockspaceOpen)
    {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        
        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }
        
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;
        
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        
        if (!opt_padding)
            ImGui::PopStyleVar();
        
        if (opt_fullscreen)
            ImGui::PopStyleVar(2);
        
        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit", NULL, false))
                    Close();
                
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }
    
    {
        ImGui::Begin("SceneHierarchy");

        if (ImGui::BeginListBox("Scene GameObjects"))
        {
            for (int n = 0; n < (int)m_activeScene->m_gameObjects.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(m_activeScene->m_gameObjects[n]->GetName().data(), is_selected))
                    item_current_idx = n;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
        
        ImGui::End();
    }

    { 
        ImGui::Begin("FrameRate");                  
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    {
        ImGui::Begin("Debugging");
        ImGui::SliderFloat("Sensivity", &s_mouseSensivity, 0.1f, 10.0f);   
        ImGui::SliderFloat("MoveSpeed", &s_moveSpeed, 0.1f, 25.0f);
        ImGui::End();
    }

    if(dockspaceOpen)
        ImGui::End(); // End dockspace

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

void LacertaEditor::Close()
{
    m_editorWindow->OnDestroy();
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
