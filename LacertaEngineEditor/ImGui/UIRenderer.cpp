#include "UIRenderer.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "imgui_src/imgui.h"
#include "Rendering/WinDX11/WinDX11Renderer.h"
#include "Rendering/WinDX11/WinDX11RenderTarget.h"
#include "UIPanels/DetailsPanel.h"
#include "UIPanels/GlobalSettingsPanel.h"
#include "UIPanels/SceneHierarchyPanel.h"
#include "imgui_src/ImGuizmo.h"

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
    
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    
    delete s_UIRenderer;
}

void UIRenderer::InitializeUI(HWND hwnd, LacertaEditor* editor)
{
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
    WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)RHI::Get()->GetRenderer(); // TODO switch on renderer type and choose the right implementation
    ImGui_ImplDX11_Init((ID3D11Device*)Dx11Renderer->GetDriver(), Dx11Renderer->GetImmediateContext());

    m_editor = editor;

    //Create and store all the main panels
    m_panels.push_back(new GlobalSettingsPanel());
    m_panels.push_back(new SceneHierarchyPanel());
    m_panels.push_back(new DetailsPanel());

    for(auto panel : m_panels)
        panel->Start();
}

void UIRenderer::Update()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

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

            ImGui::EndMenuBar();
        }
    }

    //Viewport 
    
    // TODO get the infos more properly + DX11 agnostic 
    WinDX11Renderer* Dx11Renderer = (WinDX11Renderer*)RHI::Get()->GetRenderer(); 
    WinDX11RenderTarget* SceneTextureRenderTarget = (WinDX11RenderTarget*)Dx11Renderer->GetRenderTarget(1);
    
    if(SceneTextureRenderTarget != nullptr)
    {
        if(SceneTextureRenderTarget->RenderToTexture())
        {
            {
                ImGui::Begin("Viewport");

                ImVec2 viewportSize = ImGui::GetContentRegionAvail();
                m_editor->SetViewportCachedSize(Vector2(viewportSize.x, viewportSize.y));
                ImGui::Image((void*)SceneTextureRenderTarget->GetTextureShaderResView(), viewportSize);

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

    // Stats 
    
    { 
        ImGui::Begin("FrameRate");                  
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    for(auto panel : m_panels)
        panel->Update();

    if(dockspaceOpen)
        ImGui::End(); // End dockspace

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
    
}
