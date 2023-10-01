#include "GlobalSettingsPanel.h"

#include "../UIRenderer.h"
#include "../imgui_src/imgui.h"

namespace LacertaEngineEditor
{
    
GlobalSettingsPanel::GlobalSettingsPanel()
{
}

GlobalSettingsPanel::~GlobalSettingsPanel()
{
}

void GlobalSettingsPanel::Start()
{
}

void GlobalSettingsPanel::Update()
{
    LacertaEditor* editor = UIRenderer::Get()->GetEditor();
    
    { 
        ImGui::Begin("Global Settings");
        
        ImGui::Text("Inputs");
        ImGui::SliderFloat("Mouse Sensivity", &editor->m_mouseSensivity, 0.1f, 5.0f);
        ImGui::SliderFloat("Move Speed", &editor->m_moveSpeed, 0.1f, 20.0f);
        ImGui::Separator();

        ImGui::Text("Lighting");
        ImGui::SliderFloat("Directional rotation", &editor->m_lightRotation, 0.0f, 6.28f);
        ImGui::SliderFloat("Ambient", &editor->m_ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("Diffuse", &editor->m_diffuse, 0.0f, 3.0f);
        ImGui::SliderFloat("Specular", &editor->m_specular, 0.0f, 5.0f);
        ImGui::SliderFloat("Shininess", &editor->m_shininess, 0.0f, 50.0f);
        
        ImGui::End();
    }
}

void GlobalSettingsPanel::Close()
{
}
    
}
