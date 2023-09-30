#include "LacertaEditor.h"

#include <execution>

#include "ImGui/UIRenderer.h"
#include "Rendering/Drawcall.h"

namespace LacertaEngineEditor
{

static float s_mouseSensivity = 3.0;
static float s_moveSpeed = 6.5f;
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

    // ---------------------------- Debug Scene Creation --------------------

    m_activeScene = new Scene();

    // ----------------------------- Debug GO Creation -----------------------

    // TODO standardize resource creation
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

    UIRenderer::Create();
    UIRenderer::Get()->InitializeUI(hwnd, this);

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

    // TODO Camera pos & rot are updated here with some hard coded Matrix
    
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
    
    // Update the perspective projection to the ImGui viewport size
    cc.ProjectionMatrix.SetPerspectiveFovLH(1.57f, (m_viewportCachedSize.X / m_viewportCachedSize.Y), 0.1f, 1000.0f);
    
    GraphicsEngine::Get()->UpdateShaderConstants(&cc);

    GraphicsEngine::Get()->RenderScene(m_viewportCachedSize);

    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    GraphicsEngine::Get()->SetBackbufferViewportSize(width, height);

    // ----------------------------- UI Update  --------------------------

    UIRenderer::Get()->Update();

    // ------------------------ UI & DrawCalls done, present Swap Chain now ----------------
    
    GraphicsEngine::Get()->PresentSwapChain();
}

void LacertaEditor::Quit()
{
    UIRenderer::Shutdown();
    
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
