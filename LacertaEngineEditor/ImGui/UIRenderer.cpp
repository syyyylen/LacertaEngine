#include "UIRenderer.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#include "imgui_src/imgui.h"
#include "UIPanels/DetailsPanel.h"
#include "UIPanels/GlobalSettingsPanel.h"
#include "UIPanels/SceneHierarchyPanel.h"
#include "imgui_src/ImGuizmo.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/Texture.h"
#include "Rendering/WinDX11/WinDX11Renderer.h"
#include "Rendering/WinDX12/WinDX12Renderer.h"
#include "UIPanels/TextureViewerPanel.h"

namespace LacertaEngineEditor
{
    
static int item_current_idx = 0; 

UIRenderer* UIRenderer::s_UIRenderer = nullptr;

UIRenderer::UIRenderer()
{
}

UIRenderer::~UIRenderer()
{
    s_UIRenderer = nullptr;
}

UIRenderer* UIRenderer::Get()
{
    return s_UIRenderer;
}

void UIRenderer::Create()
{
    if(s_UIRenderer)
        throw std::exception("UI Renderer already created");

    s_UIRenderer = new UIRenderer();
}

void UIRenderer::Shutdown()
{
    for(auto panel : s_UIRenderer->m_panels)
        panel->Close();

    switch (m_rendererType)
    {
    case RENDERER_WIN_DX11:
        {
            ImGui_ImplDX11_Shutdown();
            break;
        }
    
    case RENDERER_WIN_DX12:
        {
            ImGui_ImplDX12_Shutdown();
            break;
        }
    }
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    delete s_UIRenderer;
}

void UIRenderer::InitializeUI(HWND hwnd, LacertaEditor* editor, RendererType rendererType)
{
    m_rendererType = rendererType;
    
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

    switch (m_rendererType)
    {
    case RENDERER_WIN_DX11:
    {
        WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)RHI::Get()->GetRenderer();
        ImGui_ImplDX11_Init((ID3D11Device*)Dx11Renderer->GetDriver(), Dx11Renderer->GetImmediateContext());
        break;
    }
    
    case RENDERER_WIN_DX12:
    {
            WinDX12Renderer* Dx12Renderer = (WinDX12Renderer*)RHI::Get()->GetRenderer();
            auto srvHeap = Dx12Renderer->GetSrvHeap();
            auto fontDescriptor = srvHeap->Allocate();
            ImGui_ImplDX12_Init(Dx12Renderer->GetDevice(), Dx12Renderer->FramesInFlight(), Dx12Renderer->GetSwapChainFormat(),
                srvHeap->GetHeap(), fontDescriptor.CPU, fontDescriptor.GPU);
            break;
    }
    }
    
    m_editor = editor;

#if !USE_D3D12_RHI
    
    //Create and store all the main panels
    m_panels.push_back(new GlobalSettingsPanel());
    m_panels.push_back(new SceneHierarchyPanel());
    m_panels.push_back(new DetailsPanel());
    // m_panels.push_back(new TextureViewerPanel());

    for(auto panel : m_panels)
        panel->Start();

#endif
}

void UIRenderer::Update()
{
    // Start the Dear ImGui frame
    switch (m_rendererType)
    {
    case RENDERER_WIN_DX11:
        {
            ImGui_ImplDX11_NewFrame();
            break;
        }
        
    case RENDERER_WIN_DX12:
        {
            ImGui_ImplDX12_NewFrame();
            break;
        }
    }
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    // Stats 
    { 
        ImGui::Begin("FrameRate");                  
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

#if !USE_D3D12_RHI

    // Dockspace wip
    static bool dockspaceOpen = true;
    if(dockspaceOpen)
    {
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        
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
                if (ImGui::MenuItem("Save", NULL, false))
                    m_editor->SaveActiveScene();
                
                if (ImGui::MenuItem("Exit", NULL, false))
                    m_editor->Close();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Reload Shaders", NULL, false))
                    m_editor->ReloadShaders();

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    //Viewport 
    auto SceneTextureRenderTarget = RHI::Get()->GetRenderTarget(1);
    
    if(SceneTextureRenderTarget != nullptr)
    {
        if(SceneTextureRenderTarget->RenderToTexture())
        {
            {
                ImGui::Begin("Viewport");

                ImVec2 viewportSize = ImGui::GetContentRegionAvail();
                m_editor->SetViewportCachedSize(Vector2(viewportSize.x, viewportSize.y));
                ImGui::Image(SceneTextureRenderTarget->GetTargetTexture()->GetTextureSRV(), viewportSize);

                // Gizmos
                if(m_editor->HasSelectedGo())
                {
                    GameObject* selectedGo = m_editor->GetSelectedGo();

                    ImGuizmo::SetOrthographic(false);
                    ImGuizmo::SetDrawlist();

                    float windowWidth = (float)ImGui::GetWindowWidth();
                    float windowHeight = (float)ImGui::GetWindowHeight();
                    ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

                    Matrix4x4 cameraMatrix = m_editor->GetCameraMatrix();
                    cameraMatrix.Inverse(); // View matrix
                    Matrix4x4 cameraProjectionMatrix = m_editor->GetCameraProjectionMatrix();
                    auto& tfComp = selectedGo->GetComponent<TransformComponent>();
                    Matrix4x4 selectedGoTfMatrix = tfComp.GetTransformMatrix();

                    if(m_editor->m_translate)
                    {
                        ImGuizmo::Manipulate(cameraMatrix.ToFloatPtr(), cameraProjectionMatrix.ToFloatPtr(),
                        ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, selectedGoTfMatrix.ToFloatPtr());
                    }
                    else
                    {
                        ImGuizmo::Manipulate(cameraMatrix.ToFloatPtr(), cameraProjectionMatrix.ToFloatPtr(),
                        ImGuizmo::OPERATION::SCALE, ImGuizmo::LOCAL, selectedGoTfMatrix.ToFloatPtr());
                    }
                    

                    if(ImGuizmo::IsUsing())
                    {
                        tfComp.SetTransformMatrix(selectedGoTfMatrix);
                    }
                }
                
                ImGui::End();
            }
        }
    }

    for(auto panel : m_panels)
        panel->Update();

    if(dockspaceOpen)
        ImGui::End(); // End dockspace

#endif


    switch (m_rendererType)
    {
    case RENDERER_WIN_DX11:
        {
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            break;
        }
        
    case RENDERER_WIN_DX12:
        {
            auto winDX12Renderer = (WinDX12Renderer*)RHI::Get()->GetRenderer();
            auto cmdList = winDX12Renderer->GetCommandList();
            ID3D12DescriptorHeap* pHeaps[] = { winDX12Renderer->GetSrvHeap()->GetHeap() };
            cmdList->SetDescriptorHeaps(1, pHeaps);
            ImGui::Render();
            ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);
            break;
        }
    }
}
    
}
