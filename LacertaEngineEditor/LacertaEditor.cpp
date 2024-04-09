﻿#include "LacertaEditor.h"

#include <execution>

#include "ECS/Components/PointLightComponent.h"
#include "ImGui/UIRenderer.h"
#include "Rendering/ConstantBuffer.h"
#include "Rendering/Drawcall.h"
#include "Rendering/RenderPass.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/ShadowMapPassLayouts.h"
#include "Rendering/SkyBoxPassLayouts.h"
#include "Rendering/Texture.h"

#define MAX_MESHES 50

#define SHADOW_CASCADES 3

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
    InputSystem::Get()->AddListener(this);
    InputSystem::Get()->ShowCursor(false);
    
    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    InputSystem::Get()->SetCursorPosition(Vector2((float)width/2.0f, (float)height/2.0f));

    // ----------------------- Graphics Engine Creation & Renderer Initialization  ------------------------
        
    RHI::Create();
    HWND hwnd = m_editorWindow->GetHWND();
    RHI::Get()->InitializeRenderer((int*)hwnd, RendererType::RENDERER_WIN_DX11, width, height, 60);
    RHI::Get()->CreateRenderTarget(width, height, RenderTargetType::Texture2D, m_sceneRTidx);

    // Render passes
    auto scenePass = RHI::Get()->CreateRenderPass("scene");
    auto skyboxPass = RHI::Get()->CreateRenderPass("skybox");
    auto shadowMapPass = RHI::Get()->CreateRenderPass("shadowMap");
    
    scenePass->SetRenderTargetIdx(m_sceneRTidx);
    
    skyboxPass->SetRenderTargetIdx(m_sceneRTidx);
    skyboxPass->SetCullfront(true);

    RHI::Get()->CreateRenderTarget((int)m_shadowMapResolution.X, (int)m_shadowMapResolution.Y, RenderTargetType::Texture2D, m_shadowMapRTidx, SHADOW_CASCADES);
    shadowMapPass->SetRenderTargetIdx(m_shadowMapRTidx);
    shadowMapPass->SetComparisonSampling(true);
    shadowMapPass->SetCullfront(true);

    // ---------------------------- Debug Scene Creation --------------------

    m_activeScene = new Scene("Demo scene");

    // -------------------------- Skybox Sphere Creation -----------------------

    m_skyBoxGo = &AddMeshToScene("Skybox", L"Assets/Meshes/cube.obj", Vector3(0.0f, 0.0f, 0.0f));
    auto& skyboxTf = m_skyBoxGo->GetComponent<TransformComponent>();
    skyboxTf.SetScale(Vector3(1.0f, 1.0f, 1.0f));
    auto& skyBoxMeshComp = m_skyBoxGo->GetComponent<MeshComponent>();
    m_skyBoxTex = RHI::Get()->CreateTexture(L"Assets/Textures/skybox2.dds", 5);
    skyBoxMeshComp.GetMaterial()->SetTexture(0, m_skyBoxTex);
    skyBoxMeshComp.GetMaterial()->SetShader("SkyboxShader");
    
    // -------------------------- IBL Compute Shaders -----------------------

    auto renderer = RHI::Get()->GetRenderer();

    // Diffuse Irradiance
    RHI::Get()->CreateRenderTarget(128, 128, RenderTargetType::TextureCube, m_irradianceRTidx, 6);
    auto irrRT = RHI::Get()->GetRenderTarget(m_irradianceRTidx);
    m_irradianceTex = irrRT->CreateTextureFromRT(6);
    m_irradianceTex->AllowReadWrite(renderer, true);
    m_irradianceTex->Bind(renderer);
    m_skyBoxTex->Bind(renderer);
    renderer->ExecuteComputeShader("IrradianceCS", 128 / 32, 128 / 32, 6);
    m_irradianceTex->AllowReadWrite(renderer, false);

    // Pre filter Env map
    RHI::Get()->CreateRenderTarget(512, 512, RenderTargetType::TextureCube, m_prefilterRTidx, 6);
    auto preRT = RHI::Get()->GetRenderTarget(m_prefilterRTidx);
    m_prefilteredEnvMapTex = preRT->CreateTextureFromRT(7);
    m_prefilteredEnvMapTex->AllowReadWrite(renderer, true);
    m_prefilteredEnvMapTex->Bind(renderer);
    m_skyBoxTex->Bind(renderer);

    for(int i = 0; i < 5; i++)
    {
        UINT32 mipWidth = (UINT32)(512.0f * pow(0.5f, i));
        UINT32 mipHeigth = (UINT32)(512.0f * pow(0.5f, i));
        float roughness = (float)i/(float)(5 - 1);

        PrefilterMapConstantBuffer cbuf;
        cbuf.Roughness = Vector4(roughness, 0.0f, 0.0f, 0.0f);
        ConstantBuffer Cb;
        Cb.SetData(&cbuf, ConstantBufferType::PrefilterCbuf);
        Cb.Bind(renderer);

        renderer->ExecuteComputeShader("PrefilterCS", mipWidth / 32, mipHeigth / 32, 6);
    }

    m_prefilteredEnvMapTex->AllowReadWrite(renderer, false);

    // ----------------------------- Debug GO Creation -----------------------

    Vector3 spawnLocation = Vector3(0.0f, 0.0f, 0.0f);

    GameObject& sphereGo = AddMeshToScene("GregSphere", L"Assets/Meshes/spheregreg.obj", spawnLocation, "MeshPBRShader");
    TransformComponent& sphereTfComp = sphereGo.GetComponent<TransformComponent>();
    sphereTfComp.SetScale(Vector3(2.0f, 2.0f, 2.0f));
    Texture* tex = RHI::Get()->CreateTexture(L"Assets/Textures/gregcolor.png", 0);
    Texture* normalMap = RHI::Get()->CreateTexture(L"Assets/Textures/gregnormal.png", 1);
    Texture* roughnessMap = RHI::Get()->CreateTexture(L"Assets/Textures/gregroughness.png", 2);
    MeshComponent& meshComp = sphereGo.GetComponent<MeshComponent>();
    meshComp.GetMaterial()->SetTexture(0, tex);
    meshComp.GetMaterial()->SetTexture(1, normalMap);
    meshComp.GetMaterial()->SetTexture(2, roughnessMap);
    
    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);
    
    GameObject& dragonGo = AddMeshToScene("Dragon", L"Assets/Meshes/dragon.obj", spawnLocation);
    TransformComponent& dragonTfComp = dragonGo.GetComponent<TransformComponent>();
    dragonTfComp.SetScale(Vector3(1.5f, 1.5f, 1.5f));

    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& helmetGo = AddMeshToScene("DamagedHelmet", L"Assets/DamagedHelmet/DamagedHelmet.obj", spawnLocation);
    TransformComponent& helmetTfComp = helmetGo.GetComponent<TransformComponent>();
    helmetTfComp.SetScale(Vector3(8.0f, 8.0f, 8.0f));

    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& statueGo = AddMeshToScene("Statue", L"Assets/Meshes/statue.obj", spawnLocation);
    TransformComponent& statueTfComp = statueGo.GetComponent<TransformComponent>();
    statueTfComp.SetScale(Vector3(30.0f, 30.0f, 30.0f));

    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& sphere2Go = AddMeshToScene("Sphere", L"Assets/Meshes/spheregreg.obj", spawnLocation);
    TransformComponent& sphere2TfComp = sphere2Go.GetComponent<TransformComponent>();
    sphere2TfComp.SetScale(Vector3(2.0f, 2.0f, 2.0f));

    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);

    AddPBRSphereToScene("SpherePBR1", spawnLocation,
        L"Assets/Textures/PBR/limestone5_albedo.png",
        L"Assets/Textures/PBR/limestone5_Normal-ogl.png",
        L"Assets/Textures/PBR/limestone5_Roughness.png",
        L"Assets/Textures/PBR/limestone5_Metallic.png",
        L"Assets/Textures/PBR/limestone5_ao.png");
    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);
    AddPBRSphereToScene("SpherePBR2", spawnLocation,
        L"Assets/Textures/PBR/worn-factory-siding_albedo.png",
        L"Assets/Textures/PBR/worn-factory-siding_normal-ogl.png",
        L"Assets/Textures/PBR/worn-factory-siding_roughness.png",
        L"Assets/Textures/PBR/worn-factory-siding_metallic.png",
        L"Assets/Textures/PBR/worn-factory-siding_ao.png");
    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);
    AddPBRSphereToScene("SpherePBR3", spawnLocation,
        L"Assets/Textures/PBR/gold-scuffed_basecolor-boosted.png",
        L"Assets/Textures/PBR/gold-scuffed_normal.png",
        L"Assets/Textures/PBR/gold-scuffed_roughness.png",
        L"Assets/Textures/PBR/gold-scuffed_metallic.png",
        nullptr);
    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);
    AddPBRSphereToScene("SpherePBR4", spawnLocation,
        L"Assets/Textures/PBR/black-leather_albedo.png",
        L"Assets/Textures/PBR/black-leather_normal-ogl.png",
        L"Assets/Textures/PBR/black-leather_roughness.png",
        L"Assets/Textures/PBR/black-leather_metallic.png",
         L"Assets/Textures/PBR/black-leather_ao.png");
    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);
    AddPBRSphereToScene("SpherePBR4", spawnLocation,
        L"Assets/Textures/PBR/worn-shiny-metal-albedo.png",
        L"Assets/Textures/PBR/worn-shiny-metal-Normal-ogl.png",
        L"Assets/Textures/PBR/worn-shiny-metal-Roughness.png",
        L"Assets/Textures/PBR/worn-shiny-metal-Metallic.png",
         L"Assets/Textures/PBR/worn-shiny-metal-ao.png");
    
    GameObject& groundGo = AddMeshToScene("Ground", L"Assets/Meshes/cube.obj", Vector3(130.0f, -16.0f, 9.0f));
    TransformComponent& groundGoTf = groundGo.GetComponent<TransformComponent>();
    groundGoTf.SetScale(Vector3(160.0f, 1.5f, 100.0f));
    auto cubeMeshComp = groundGo.GetComponent<MeshComponent>();
    cubeMeshComp.GetMaterial()->SetCastShadow(false);

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

    // Directional light shadow map pass ---------------------------------------------------------------------------
    auto shadowMapPass = RHI::Get()->GetRenderPass("shadowMap");
    shadowMapPass->ClearDrawcalls();
    
    ConstantBuffer MeshesBufs[MAX_MESHES];
    int j = 0;
    auto tfMeshesGrp = m_activeScene->m_registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto go : tfMeshesGrp)
    {
        auto[transform, meshComponent] = tfMeshesGrp.get<TransformComponent, MeshComponent>(go);
        if(meshComponent.GetMaterial() == nullptr || meshComponent.GetMaterial()->GetShader() == "SkyboxShader"
            || !meshComponent.GetMaterial()->CastShadow()) // TODO skybox GO doesn't belong to this pass
            continue;

        Mesh* mesh = meshComponent.GetMesh();
        auto shapes = mesh->GetShapesData();
        for(const auto shape : shapes)
        {
            std::vector<Bindable*> bindables;
            SceneMeshConstantBuffer* meshCb = new SceneMeshConstantBuffer(); // this is deleted by CBuf
            meshCb->LocalMatrix = transform.GetTransformMatrix();
            MeshesBufs[j].SetData(meshCb, ConstantBufferType::MeshCbuf);
            bindables.emplace_back(&MeshesBufs[j]);
            j++;
            shadowMapPass->AddDrawcall("ShadowMapShader", shape, bindables);
        }
    }

    auto shadowMapCC = new ShadowMapLightConstantBuffer();
    
    for(int i = 0; i < SHADOW_CASCADES; i++)
    {
        shadowMapPass->ClearGlobalBindables();

        auto smCC = new ShadowMapLightConstantBuffer();
        
        Matrix4x4 shadowMapView;
        Matrix4x4 m;
        shadowMapView.SetIdentity();
        m.SetIdentity();
        m.SetRotationX(m_lightRotationX);
        shadowMapView *= m;
        m.SetIdentity();
        m.SetRotationY(m_lightRotationY);
        shadowMapView *= m;
        shadowMapView.SetTranslation(m_sceneCamera.GetTranslation());
        shadowMapView.Inverse();
        smCC->ViewMatrix[0] = shadowMapView;
        
        float size; // TODO clean all this
        if(i == 0) size = 100.0;
        if(i == 1) size = 300.0;
        if(i == 2) size = 1000.0;
        smCC->ProjectionMatrix[0].SetOrthoLH(size, size, -200.0f, 200.0f);

        shadowMapCC->ViewMatrix[i] = shadowMapView;
        shadowMapCC->ProjectionMatrix[i].SetOrthoLH(size, size, -200.0f, 200.0f);

        ConstantBuffer shadowMapCbuf = ConstantBuffer(smCC, ConstantBufferType::SMLightCbuf);
        shadowMapPass->AddGlobalBindable(&shadowMapCbuf);

        shadowMapPass->SetRenderTargetSubresourceIdx(i);
        RHI::Get()->ExecuteRenderPass("shadowMap", m_shadowMapResolution, true);
    }

    // ---------------------------------------------------------------------------------------------------------
    
    // TODO Camera pos & rot are updated here with some hard coded Matrix
    
    // Constant Buffer Update
    auto cc = new SceneConstantBuffer();
    auto skyboxCC = new SkyBoxConstantBuffer();
    cc->Time = m_globalTimer->Elapsed(); 
    
    cc->WorldMatrix.SetIdentity();
    
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
    cc->CameraPosition = newCamPos;
    m_sceneCamera = worldCam;
    worldCam.Inverse();
    cc->ViewMatrix = worldCam;
    skyboxCC->ViewMatrix = worldCam;
    
    // Update the perspective projection to the ImGui viewport size
    cc->ProjectionMatrix.SetPerspectiveFovLH(1.57f, (m_viewportCachedSize.X / m_viewportCachedSize.Y), 0.1f, 5000.0f);
    skyboxCC->ProjectionMatrix.SetPerspectiveFovLH(1.57f, (m_viewportCachedSize.X / m_viewportCachedSize.Y), 0.1f, 5000.0f);
    m_sceneCameraProjection = cc->ProjectionMatrix;

    // Ambient lighting constant
    cc->GlobalAmbient = m_ambient;
    skyboxCC->GlobalAmbient = m_ambient;

    // Directional Light set up
    Matrix4x4 lightRotationMatrix;
    lightRotationMatrix.SetIdentity();
    if(m_directionalLightAutoRotate)
        m_lightRotationY = m_lightRotationY + m_deltaTime * m_directionalLightAutoRotateScalar;

    temp.SetIdentity();
    temp.SetRotationX(m_lightRotationX);
    lightRotationMatrix *= temp;
    temp.SetIdentity();
    temp.SetRotationY(m_lightRotationY);
    lightRotationMatrix *= temp;
    
    cc->DirectionalLightDirection = lightRotationMatrix.GetZDirection();
    cc->DirectionalIntensity = m_lightIntensity;

    for(int i = 0; i < MAX_LIGHTS; i++)
    {
        cc->PointLights[i].Enabled = false;
    }
    
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

        cc->PointLights[i] = pointLight;
    }
    
    cc->DefaultColor = m_defaultColor;

    auto scenePass = RHI::Get()->GetRenderPass("scene");
    auto skyboxPass = RHI::Get()->GetRenderPass("skybox");

    scenePass->ClearGlobalBindables();
    skyboxPass->ClearGlobalBindables();
    
    ConstantBuffer sceneCbuf = ConstantBuffer(cc, ConstantBufferType::SceneCbuf);
    scenePass->AddGlobalBindable(&sceneCbuf);

    scenePass->AddGlobalBindable(m_irradianceTex);
    scenePass->AddGlobalBindable(m_prefilteredEnvMapTex);
    scenePass->AddGlobalBindable(m_skyBoxTex);
    auto shadowMapRT = RHI::Get()->GetRenderTarget(m_shadowMapRTidx);
    auto shadowMap = shadowMapRT->CreateTextureFromDepth(8);
    scenePass->AddGlobalBindable(shadowMap);
    ConstantBuffer shadowMapCbuf = ConstantBuffer(shadowMapCC, ConstantBufferType::SMLightCbuf);
    scenePass->AddGlobalBindable(&shadowMapCbuf);

    ConstantBuffer skyboxCbuf = ConstantBuffer(skyboxCC, ConstantBufferType::SkyBoxCbuf);
    skyboxPass->AddGlobalBindable(&skyboxCbuf);

    // ----------------------------- Scene Draw Objects -----------------------

    scenePass->ClearDrawcalls();
    skyboxPass->ClearDrawcalls();

    // We stack allocate an array of cbufs for each meshes in the scene
    ConstantBuffer MeshesBuffers[MAX_MESHES];

    int i = 0;
    auto tfMeshesGroup = m_activeScene->m_registry.group<TransformComponent>(entt::get<MeshComponent>);
    for(auto go : tfMeshesGroup)
    {
        auto[transform, meshComponent] = tfMeshesGroup.get<TransformComponent, MeshComponent>(go);

        if(meshComponent.GetMaterial() == nullptr || meshComponent.GetMaterial()->GetShader() == "SkyboxShader") // TODO skybox GO doesn't belong to this pass
            continue;

        Mesh* mesh = meshComponent.GetMesh();
        auto shapes = mesh->GetShapesData();
        for(const auto shape : shapes)
        {
            auto mat = meshComponent.GetMaterial();
            auto texs = mat->GetTextures();
            std::vector<Bindable*> bindables;
            for(auto tex : texs)
                bindables.emplace_back(tex);

            SceneMeshConstantBuffer* meshCb = new SceneMeshConstantBuffer(); // this is deleted by CBuf

            int texLength = (int)texs.size(); // TODO fix all this
            meshCb->HasAlbedo = texLength > 0;
            meshCb->HasNormalMap = texLength > 1;
            meshCb->HasRoughness = texLength > 2;
            meshCb->HasMetallic = texLength > 3;
            
            meshCb->LightProperties = mat->GetMatLightProperties();
            meshCb->LocalMatrix = transform.GetTransformMatrix();

            MeshesBuffers[i].SetData(meshCb, ConstantBufferType::MeshCbuf);
            bindables.emplace_back(&MeshesBuffers[i]);
            i++;
            
            scenePass->AddDrawcall(mat->GetShader(), shape, bindables);
        }
    }

    auto skyboxMeshComp = m_skyBoxGo->GetComponent<MeshComponent>();
    auto skyboxMesh = skyboxMeshComp.GetMesh();
    auto shapes = skyboxMesh->GetShapesData();
    for(const auto shape : shapes)
    {
        auto mat = skyboxMeshComp.GetMaterial();
        auto texs = mat->GetTextures();
        
        std::vector<Bindable*> bindables;
        for(auto tex : texs)
            bindables.emplace_back(tex);
    
        skyboxPass->AddDrawcall(mat->GetShader(), shape, bindables);
    }

    RHI::Get()->ExecuteRenderPass("scene", m_viewportCachedSize, true);
    RHI::Get()->ExecuteRenderPass("skybox", m_viewportCachedSize, false);

    // We get the backbuffer back to render UI in it
    RHI::Get()->SetBackbufferRenderTargetActive();

    RECT windowRect = m_editorWindow->GetClientWindowRect();
    int width = windowRect.right - windowRect.left;
    int height = windowRect.bottom - windowRect.top;
    RHI::Get()->SetBackbufferViewportSize(width, height);

    // ----------------------------- UI Update  --------------------------

    UIRenderer::Get()->Update();

    // ------------------------ UI & DrawCalls done, present Swap Chain now ----------------
    
    RHI::Get()->PresentSwapChain();
}

void LacertaEditor::Quit()
{
    UIRenderer::Shutdown();
    
    if(m_editorWindow)
        m_editorWindow->Destroy();

    RHI::Shutdown();
    
    InputSystem::Get()->RemoveListener(this);
    InputSystem::Release();

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

void LacertaEditor::SaveActiveScene()
{
    if(m_activeScene == nullptr)
        return;

    SceneSerializer Serializer;
    Serializer.Serialize(*m_activeScene, nullptr);
}

Scene* LacertaEditor::LoadSceneFromFile(const wchar_t* filePath)
{
    // TODO parse scene file and create scene obj fully loaded from it
    
    return nullptr;
}

void LacertaEditor::ReloadShaders()
{
    RHI::Get()->GetRenderer()->LoadShaders();
}

void LacertaEditor::DestroyGo(GameObject* goToDestroy)
{
    if(goToDestroy == m_selectedObject)
        m_selectedObject = nullptr;

    if(m_activeScene == nullptr)
        return;
    
    m_activeScene->RemoveGameObject(goToDestroy);
}

GameObject& LacertaEditor::AddMeshToScene(std::string name, const wchar_t* meshPath, Vector3 position, std::string shader)
{
    Mesh* mesh = RHI::Get()->CreateMesh(meshPath);

    name += "_";
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
    std::string name = "PointLight_";
    name += std::to_string(m_activeScene->m_gameObjects.size());
    GameObject* go = m_activeScene->CreateGameObject(name, position);

    PointLightComponent& lightComp = go->AddComponent<PointLightComponent>();
    lightComp.SetColor(color);

    return *go;
}

GameObject& LacertaEditor::AddPBRSphereToScene(std::string name,
    Vector3 position,
    const wchar_t* albedo,
    const wchar_t* normal,
    const wchar_t* roughness,
    const wchar_t* metallic,
    const wchar_t* ao)
{
    GameObject& sphere = AddMeshToScene(name, L"Assets/Meshes/sphere_hq.obj", position);
    
    auto& sphereTf = sphere.GetComponent<TransformComponent>();
    sphereTf.SetScale(Vector3(12.0f, 12.0f, 12.0f));
    auto tex = RHI::Get()->CreateTexture(albedo, 0);
    auto norm = RHI::Get()->CreateTexture(normal, 1);
    auto rough = RHI::Get()->CreateTexture(roughness, 2);
    auto met = RHI::Get()->CreateTexture(metallic, 3);
    auto& sphereMesh = sphere.GetComponent<MeshComponent>();
    sphereMesh.GetMaterial()->SetTexture(0, tex);
    sphereMesh.GetMaterial()->SetTexture(1, norm);
    sphereMesh.GetMaterial()->SetTexture(2, rough);
    sphereMesh.GetMaterial()->SetTexture(3, met);
    if(ao != nullptr)
    {
        auto amb = RHI::Get()->CreateTexture(ao, 4);
        sphereMesh.GetMaterial()->SetTexture(4, amb);
    }

    return sphere;
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
