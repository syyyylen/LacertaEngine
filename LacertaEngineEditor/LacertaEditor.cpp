#include "LacertaEditor.h"

#include <execution>

#include "ECS/Components/PointLightComponent.h"
#include "ImGui/UIRenderer.h"
#include "Rendering/Drawcall.h"

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

    // TODO this needs to be created first bc we want it to be rendered first. Isn't it stupid ? (rhetorical question, it is.)
    // -------------------------- Skybox Sphere Creation -----------------------

    m_skyBoxGo = &AddMeshToScene(L"Assets/Meshes/sphere_hq.obj", Vector3(0.0f, 0.0f, 0.0f));
    auto& skyboxTf = m_skyBoxGo->GetComponent<TransformComponent>();
    skyboxTf.SetScale(Vector3(500.0f, 500.0f, 500.0f));
    auto& skyBoxMeshComp = m_skyBoxGo->GetComponent<MeshComponent>();
    SkyBoxTexture* skyBoxTex = ResourceManager::Get()->CreateResource<SkyBoxTexture>(L"Assets/Textures/skymap.dds");
    skyBoxMeshComp.GetMaterial()->SetSkyBoxTexture(skyBoxTex);
    skyBoxMeshComp.GetMaterial()->SetShader("SkyboxShader");
    skyBoxMeshComp.GetMaterial()->SetIsSkyBox(true);

    // ----------------------------- Debug GO Creation -----------------------

    Vector3 spawnLocation = Vector3(0.0f, 0.0f, 0.0f);

    GameObject& sphereGo = AddMeshToScene(L"Assets/Meshes/spheregreg.obj", spawnLocation, "MeshPBRShader");
    TransformComponent& sphereTfComp = sphereGo.GetComponent<TransformComponent>();
    sphereTfComp.SetScale(Vector3(2.0f, 2.0f, 2.0f));
    Texture* tex = ResourceManager::Get()->CreateResource<Texture>(L"Assets/Textures/gregcolor.png");
    Texture* normalMap = ResourceManager::Get()->CreateResource<Texture>(L"Assets/Textures/gregnormal.png");
    MeshComponent& meshComp = sphereGo.GetComponent<MeshComponent>();
    meshComp.GetMaterial()->SetTexture(0, tex);
    meshComp.GetMaterial()->SetTexture(1, normalMap);
    
    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);
    
    GameObject& dragonGo = AddMeshToScene(L"Assets/Meshes/dragon.obj", spawnLocation);
    TransformComponent& dragonTfComp = dragonGo.GetComponent<TransformComponent>();
    dragonTfComp.SetScale(Vector3(1.5f, 1.5f, 1.5f));

    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& teapotGo = AddMeshToScene(L"Assets/Meshes/teapot.obj", spawnLocation);
    TransformComponent& teapotTfComp = teapotGo.GetComponent<TransformComponent>();
    teapotTfComp.SetScale(Vector3(10.0f, 10.0f, 10.0f));

    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& statueGo = AddMeshToScene(L"Assets/Meshes/statue.obj", spawnLocation);
    TransformComponent& statueTfComp = statueGo.GetComponent<TransformComponent>();
    statueTfComp.SetScale(Vector3(30.0f, 30.0f, 30.0f));

    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& sphere2Go = AddMeshToScene(L"Assets/Meshes/spheregreg.obj", spawnLocation);
    TransformComponent& sphere2TfComp = sphere2Go.GetComponent<TransformComponent>();
    sphere2TfComp.SetScale(Vector3(2.0f, 2.0f, 2.0f));

    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& sphere3Go = AddMeshToScene(L"Assets/Meshes/sphere_hq.obj", spawnLocation);
    auto& sphere3TfComp = sphere3Go.GetComponent<TransformComponent>();
    sphere3TfComp.SetScale(Vector3(12.0f, 12.0f, 12.0f));
    auto brickTex = ResourceManager::Get()->CreateResource<Texture>(L"Assets/Textures/brick_d.jpg");
    auto brickNormal = ResourceManager::Get()->CreateResource<Texture>(L"Assets/Textures/brick_n.jpg");
    auto& sphere3MeshComp = sphere3Go.GetComponent<MeshComponent>();
    sphere3MeshComp.GetMaterial()->SetTexture(0, brickTex);
    sphere3MeshComp.GetMaterial()->SetTexture(1, brickNormal);
    
    GameObject& groundGo = AddMeshToScene(L"Assets/Meshes/cube.obj", Vector3(80.0f, -14.0f, 0.0f));
    TransformComponent& groundGoTf = groundGo.GetComponent<TransformComponent>();
    groundGoTf.SetScale(Vector3(145.0f, 0.5f, 45.0f));

    // -------------------------- Adding Point Lights --------------------------

    AddPointLightToScene(Vector3(60.0f, 8.0f, 20.0f));
    AddPointLightToScene(Vector3(8.0f, 9.0f, -14.0f));

    // --------------------------- Camera Default Position ---------------------

    m_sceneCamera.SetTranslation(Vector3(0.0f, 15.0f, -35.0f));

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

    Vector3 newCamPos = m_sceneCamera.GetTranslation() + worldCam.GetZDirection() * (m_cameraForward * (m_moveSpeed * m_inputDownScalar));
    newCamPos = newCamPos + worldCam.GetXDirection() * (m_cameraRight * (m_moveSpeed * m_inputDownScalar));
    worldCam.SetTranslation(newCamPos);
    cc.CameraPosition = newCamPos;
    m_sceneCamera = worldCam;
    worldCam.Inverse();
    cc.ViewMatrix = worldCam;
    
    // Update the perspective projection to the ImGui viewport size
    cc.ProjectionMatrix.SetPerspectiveFovLH(1.57f, (m_viewportCachedSize.X / m_viewportCachedSize.Y), 0.1f, 3000.0f);
    m_sceneCameraProjection = cc.ProjectionMatrix;

    // Ambient lighting constant
    cc.GlobalAmbient = m_ambient;

    // Directional Light set up
    Matrix4x4 lightRotationMatrix;
    lightRotationMatrix.SetIdentity();
    if(m_directionalLightAutoRotate)
        m_lightRotationY = m_lightRotationY + m_deltaTime * m_directionalLightAutoRotateScalar;

    temp.SetIdentity();
    temp.SetRotationY(m_lightRotationY);
    lightRotationMatrix *= temp;
    temp.SetIdentity();
    temp.SetRotationX(m_lightRotationX);
    lightRotationMatrix *= temp;
    
    cc.DirectionalLightDirection = lightRotationMatrix.GetZDirection();
    cc.DirectionalIntensity = m_lightIntensity;

    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        cc.PointLights[i].Enabled = false;
    }
    
    // TODO remove this, render the skybox mesh with a different approach 
    // We move the sphere skybox at camera pos
    auto& skyboxTf = m_skyBoxGo->GetComponent<TransformComponent>();
    skyboxTf.SetPosition(m_sceneCamera.GetTranslation());

    // Let's add the point lights to the Constant Buffer
    const auto pointLightsView = m_activeScene->m_registry.group<PointLightComponent>(entt::get<TransformComponent>);
    for(int i = 0; i < pointLightsView.size(); i++)
    {
        if(i > MAX_LIGHTS)
            break;

        const auto pointLightGo = pointLightsView[i];
        auto[transform, pointLightComp] = pointLightsView.get<TransformComponent, PointLightComponent>(pointLightGo);
        
        PointLight pointLight;
        pointLight.Enabled = true;
        pointLight.Position = transform.Position();
        pointLight.Color = pointLightComp.GetColor();
        pointLight.ConstantAttenuation = pointLightComp.GetConstantAttenuation() * 0.1f;
        pointLight.LinearAttenuation = pointLightComp.GetLinearAttenuation() * 0.01f;
        pointLight.QuadraticAttenuation = pointLightComp.GetQuadraticAttenuation() * 0.01f;

        cc.PointLights[i] = pointLight;
    }
    
    cc.DefaultColor = m_defaultColor;
    
    GraphicsEngine::Get()->UpdateShaderConstants(&cc);

    // ----------------------------- Scene Draw Objects -----------------------

    GraphicsEngine::Get()->ClearDrawcalls();

    auto tfMeshesGroup = m_activeScene->m_registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto go : tfMeshesGroup)
    {
        auto[transform, meshComponent] = tfMeshesGroup.get<TransformComponent, MeshComponent>(go);

        if(meshComponent.GetMaterial() == nullptr)
            continue;

        Mesh* mesh = meshComponent.GetMesh();
        auto shapes = mesh->GetShapesData();
        for(const auto shape : shapes)
        {
            // Adding DC
            DrawcallData dcData = {};
            dcData.VBO = shape.Vbo;
            dcData.VerticesCount = shape.VerticesSize;
            dcData.IBO = shape.Ibo;
            dcData.IndicesCount = shape.IndexesSize;
            dcData.Type = DrawcallType::dcMesh;
            dcData.LocalMatrix = transform.GetTransformMatrix();
            dcData.Material = meshComponent.GetMaterial();

            GraphicsEngine::Get()->AddDrawcall(&dcData);
        }
    }

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

void LacertaEditor::DestroyGo(GameObject* goToDestroy)
{
    if(goToDestroy == m_selectedObject)
        m_selectedObject = nullptr;

    if(m_activeScene == nullptr)
        return;
    
    m_activeScene->RemoveGameObject(goToDestroy);
}

GameObject& LacertaEditor::AddMeshToScene(const wchar_t* meshPath, Vector3 position, std::string shader)
{
    Mesh* mesh = ResourceManager::Get()->CreateResource<Mesh>(meshPath);
    
    std::string name = "GameObject";
    name += std::to_string(m_activeScene->m_gameObjects.size());
    GameObject* go = m_activeScene->CreateGameObject(name, position);
    
    MeshComponent& meshComp = go->AddComponent<MeshComponent>();
    meshComp.SetMesh(mesh);
    
    Material* newMat = new Material();
    MatLightProperties properties;
    properties.Shininess = 0.0f; // TODO shininess is 10 by default, find why
    newMat->InitializeProperties(properties, shader);
    meshComp.SetMaterial(newMat);

    return *go;
}

GameObject& LacertaEditor::AddPointLightToScene(Vector3 position, Vector4 color)
{
    std::string name = "PointLight";
    name += std::to_string(m_activeScene->m_gameObjects.size());
    GameObject* go = m_activeScene->CreateGameObject(name, position);

    PointLightComponent& lightComp = go->AddComponent<PointLightComponent>();
    lightComp.SetColor(color);

    return *go;
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

    if(key == 'R')
    {
        m_translate = !m_translate;
    }

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
    
    m_cameraRotationX += (mousePosition.Y - (height / 2.0f)) * m_deltaTime * (m_mouseSensivity * m_inputDownScalar);
    m_cameraRotationY += (mousePosition.X - (width / 2.0f)) * m_deltaTime * (m_mouseSensivity * m_inputDownScalar);

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
