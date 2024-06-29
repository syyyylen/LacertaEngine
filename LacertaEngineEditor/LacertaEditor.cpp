#include "LacertaEditor.h"

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

    HWND hwnd = m_editorWindow->GetHWND();
    RHI::Create();

#if USE_D3D12_RHI

    // TODO temp simplifed init of D3D12
    RHI::Get()->InitializeRenderer((int*)hwnd, RendererType::RENDERER_WIN_DX12, width, height, 60);

#else // D3D11 path
    
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

    m_skyBoxGo = &ImportMeshToScene("Skybox", "Assets/Meshes/cube.obj", Vector3(0.0f, 0.0f, 0.0f));
    auto& skyboxTf = m_skyBoxGo->GetComponent<TransformComponent>();
    skyboxTf.SetScale(Vector3(1.0f, 1.0f, 1.0f));
    auto& skyBoxMeshComp = m_skyBoxGo->GetComponent<MeshComponent>();
    m_skyBoxTex = RHI::Get()->CreateTexture(L"Assets/Textures/skybox2.dds", 5);
    skyBoxMeshComp.GetMaterial()->SetTexture(0, m_skyBoxTex);
    skyBoxMeshComp.GetMaterial()->SetShader("SkyboxShader");
    
    // -------------------------- IBL Compute Shaders -----------------------

    auto renderer = RHI::Get()->GetRenderer();

    // Diffuse Irradiance
    m_irradianceTex = RHI::Get()->CreateTexture(128, 128, TextureType::TexCube, 6, 1, TextureBindFlags::SRV | TextureBindFlags::UAV);
    m_irradianceTex->SetTextureIdx(6);
    m_irradianceTex->AllowReadWrite(renderer, true, 0);
    m_irradianceTex->Bind(renderer);
    m_skyBoxTex->Bind(renderer);
    renderer->ExecuteComputeShader("IrradianceCS", 128 / 32, 128 / 32, 6);
    m_irradianceTex->AllowReadWrite(renderer, false, 0);

    // Pre filter Env map
    m_prefilteredEnvMapTex = RHI::Get()->CreateTexture(512, 512, TextureType::TexCube, 6, 5, TextureBindFlags::SRV | TextureBindFlags::UAV);
    m_prefilteredEnvMapTex->SetTextureIdx(7);
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

        m_prefilteredEnvMapTex->AllowReadWrite(renderer, true, i); // We allow rw for the mip idx
        m_prefilteredEnvMapTex->Bind(renderer);
        renderer->ExecuteComputeShader("PrefilterCS", mipWidth / 32, mipHeigth / 32, 6);
    }

    m_prefilteredEnvMapTex->AllowReadWrite(renderer, false, 0);

    // BRDF
    m_BRDFTex = RHI::Get()->CreateTexture(512, 512, TextureType::Tex2D, 1, 1, TextureBindFlags::SRV | TextureBindFlags::UAV);
    m_BRDFTex->SetTextureIdx(9);
    m_BRDFTex->AllowReadWrite(renderer, true, 0);
    m_BRDFTex->Bind(renderer);
    m_skyBoxTex->Bind(renderer);
    renderer->ExecuteComputeShader("BRDFCS", 512 / 32, 512 / 32, 1);
    m_BRDFTex->AllowReadWrite(renderer, false, 0);

    // ----------------------------- Debug GO Creation -----------------------

    Vector3 spawnLocation = Vector3(0.0f, 0.0f, 0.0f);

    GameObject& sphereGo = ImportMeshToScene("GregSphere", "Assets/Meshes/spheregreg.obj", spawnLocation, "MeshPBRShader");
    sphereGo.GetComponent<TransformComponent>().SetScale(Vector3(2.0f, 2.0f, 2.0f));
    SetGameObjectPBRTextures(sphereGo, L"Assets/Textures/gregcolor.png", L"Assets/Textures/gregnormal.png", L"Assets/Textures/gregroughness.png");
    
    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);
    
    GameObject& dragonGo = ImportMeshToScene("Dragon", "Assets/Meshes/dragon.obj", spawnLocation);
    dragonGo.GetComponent<TransformComponent>().SetScale(Vector3(1.5f, 1.5f, 1.5f));

    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& helmetGo = ImportMeshToScene("DamagedHelmet", "Assets/Meshes/DamagedHelmet.gltf", spawnLocation);
    helmetGo.GetComponent<TransformComponent>().SetScale(Vector3(8.0f, 8.0f, 8.0f));
    SetGameObjectPBRTextures(helmetGo,
        L"Assets/Meshes/DamagedHelmet_albedo.jpg",
        L"Assets/Meshes/DamagedHelmet_normal.jpg",
        L"Assets/Meshes/DamagedHelmet_metalRoughness.jpg",
        nullptr,
        L"Assets/Meshes/DamagedHelmet_AO.jpg",
        L"Assets/Meshes/DamagedHelmet_emissive.jpg",
        true);
    
    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& sciFiHelmetGo = ImportMeshToScene("SciFiHelmet", "Assets/Meshes/SciFiHelmet.gltf", spawnLocation);
    sciFiHelmetGo.GetComponent<TransformComponent>().SetScale(Vector3(8.0f, 8.0f, 8.0f));
    SetGameObjectPBRTextures(sciFiHelmetGo,
        L"Assets/Meshes/SciFiHelmet_BaseColor.png",
        L"Assets/Meshes/SciFiHelmet_Normal.png",
        L"Assets/Meshes/SciFiHelmet_MetallicRoughness.png",
        nullptr,
        L"Assets/Meshes/SciFiHelmet_AmbientOcclusion.png",
        nullptr,
        true);
    
    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& cerberus = ImportMeshToScene("Cerberus", "Assets/Meshes/Cerberus.gltf", spawnLocation);
    cerberus.GetComponent<TransformComponent>().SetScale(Vector3(15.0f, 15.0f, 15.0f));
    SetGameObjectPBRTextures(cerberus,
        L"Assets/Meshes/Cerberus_Albedo.png",
        L"Assets/Meshes/Cerberus_Normal.png",
        L"Assets/Meshes/Cerberus_MetallicRoughness.png",
        nullptr,
        L"Assets/Meshes/Cerberus_AO.png",
        nullptr,
        true);

    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& statueGo = ImportMeshToScene("Statue", "Assets/Meshes/statue.obj", spawnLocation + Vector3(0.0f, -4.5f, 0.0f));
    statueGo.GetComponent<TransformComponent>().SetScale(Vector3(40.0f, 40.0f, 40.0f));
    
    spawnLocation = Vector3(spawnLocation.X + 25.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& sphere2Go = ImportMeshToScene("Sphere", "Assets/Meshes/sphere.gltf", spawnLocation);
    sphere2Go.GetComponent<TransformComponent>().SetScale(Vector3(12.0f, 12.0f, 12.0f));
    
    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& spherePBR1Go = ImportMeshToScene("Sphere", "Assets/Meshes/sphere.gltf", spawnLocation);
    spherePBR1Go.GetComponent<TransformComponent>().SetScale(Vector3(12.0f, 12.0f, 12.0f));
    SetGameObjectPBRTextures(spherePBR1Go,
    L"Assets/Textures/PBR/limestone5_albedo.png",
    L"Assets/Textures/PBR/limestone5_Normal-ogl.png",
    L"Assets/Textures/PBR/limestone5_Roughness.png",
    L"Assets/Textures/PBR/limestone5_Metallic.png",
    L"Assets/Textures/PBR/limestone5_ao.png");

    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& spherePBR2Go = ImportMeshToScene("Sphere", "Assets/Meshes/sphere.gltf", spawnLocation);
    spherePBR2Go.GetComponent<TransformComponent>().SetScale(Vector3(12.0f, 12.0f, 12.0f));
    SetGameObjectPBRTextures(spherePBR2Go,
    L"Assets/Textures/PBR/worn-factory-siding_albedo.png",
    L"Assets/Textures/PBR/worn-factory-siding_normal-ogl.png",
    L"Assets/Textures/PBR/worn-factory-siding_roughness.png",
    L"Assets/Textures/PBR/worn-factory-siding_metallic.png",
    L"Assets/Textures/PBR/worn-factory-siding_ao.png");

    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& spherePBR3Go = ImportMeshToScene("Sphere", "Assets/Meshes/sphere.gltf", spawnLocation);
    spherePBR3Go.GetComponent<TransformComponent>().SetScale(Vector3(12.0f, 12.0f, 12.0f));
    SetGameObjectPBRTextures(spherePBR3Go,
    L"Assets/Textures/PBR/gold-scuffed_basecolor-boosted.png",
    L"Assets/Textures/PBR/gold-scuffed_normal.png",
    L"Assets/Textures/PBR/gold-scuffed_roughness.png",
    L"Assets/Textures/PBR/gold-scuffed_metallic.png");

    spawnLocation = Vector3(spawnLocation.X + 30.0f, spawnLocation.Y, spawnLocation.Z);

    GameObject& spherePBR4Go = ImportMeshToScene("Sphere", "Assets/Meshes/sphere.gltf", spawnLocation);
    spherePBR4Go.GetComponent<TransformComponent>().SetScale(Vector3(12.0f, 12.0f, 12.0f));
    SetGameObjectPBRTextures(spherePBR4Go,
    L"Assets/Textures/PBR/worn-shiny-metal-albedo.png",
    L"Assets/Textures/PBR/worn-shiny-metal-Normal-ogl.png",
    L"Assets/Textures/PBR/worn-shiny-metal-Roughness.png",
    L"Assets/Textures/PBR/worn-shiny-metal-Metallic.png",
     L"Assets/Textures/PBR/worn-shiny-metal-ao.png");

    GameObject& groundGo = ImportMeshToScene("Ground", "Assets/Meshes/cube.obj", Vector3(150.0f, -16.0f, 9.0f));
    TransformComponent& groundGoTf = groundGo.GetComponent<TransformComponent>();
    groundGoTf.SetScale(Vector3(200.0f, 1.5f, 100.0f));
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

#endif

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

#if USE_D3D12_RHI

    // TODO temp simplifed update of D3D12

#else // D3D11 path

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
    scenePass->AddGlobalBindable(m_BRDFTex);
    auto shadowMapRT = RHI::Get()->GetRenderTarget(m_shadowMapRTidx);
    auto shadowMap = shadowMapRT->CreateTextureFromDepth();
    shadowMap->SetTextureIdx(8);
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

            // TODO clean all this mess
            meshCb->HasAlbedo = mat->hasAlbedo;
            meshCb->HasNormalMap = mat->HasNormal;
            meshCb->HasRoughness = mat->HasRoughness;
            meshCb->HasMetallic = mat->HasMetallic;
            meshCb->HasAmbiant = mat->HasAmbiant;
            meshCb->HasEmissive = mat->HasEmissive;
            meshCb->HasMetallicRoughness = mat->hasMR;
            
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

#endif // D3D11 path

}

void LacertaEditor::Quit()
{
    
#if !USE_D3D12_RHI
    UIRenderer::Shutdown();
#endif
    
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

GameObject& LacertaEditor::ImportMeshToScene(std::string name, std::string meshPathStr, Vector3 position, std::string shader)
{
    auto Renderer = RHI::Get()->GetRenderer();
    auto mesh = Renderer->ImportMesh(meshPathStr);

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

void LacertaEditor::SetGameObjectPBRTextures(GameObject& go,
    const wchar_t* albedo,
    const wchar_t* normal,
    const wchar_t* roughness,
    const wchar_t* metallic,
    const wchar_t* ao,
    const wchar_t* emissive, bool mr)
{
    auto& meshComp = go.GetComponent<MeshComponent>();
    auto mat = meshComp.GetMaterial();
    if(albedo)
    {
        auto tex = RHI::Get()->CreateTexture(albedo, 0);
        mat->SetTexture(0, tex);
        mat->hasAlbedo = true;
    }
    if(normal)
    {
        auto norm = RHI::Get()->CreateTexture(normal, 1);
        mat->SetTexture(1, norm);
        mat->HasNormal = true;
    }
    if(mr)
    {
        auto mrTex = RHI::Get()->CreateTexture(roughness, 2);
        mat->SetTexture(2, mrTex);
        mat->hasMR = true;
    }
    else
    {
        if(roughness)
        {
            auto rough = RHI::Get()->CreateTexture(roughness, 2);
            mat->SetTexture(2, rough);
            mat->HasRoughness = true;
        }
        if(metallic)
        {
            auto met = RHI::Get()->CreateTexture(metallic, 3);
            mat->SetTexture(3, met);
            mat->HasMetallic = true;
        }
    }
    if(ao)
    {
        auto amb = RHI::Get()->CreateTexture(ao, 4);
        mat->SetTexture(4, amb);
        mat->HasAmbiant = true;
    }
    if(emissive)
    {
        auto em = RHI::Get()->CreateTexture(emissive, 10);
        mat->SetTexture(5, em);
        mat->HasEmissive = true;
    }
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
