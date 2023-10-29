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
        ImGui::Checkbox("Direction auto rotate", &editor->m_directionalLightAutoRotate);
        ImGui::SliderFloat("Directional intensity", &editor->m_lightIntensity, 0.0f, 1.0f);
        ImGui::SliderFloat("Directional rotation X", &editor->m_lightRotationX, 0.0f, 6.28f);
        ImGui::SliderFloat("Directional rotation Y", &editor->m_lightRotationY, 0.0f, 6.28f);
        ImGui::SliderFloat("Directional auto rotate speed", &editor->m_directionalLightAutoRotateScalar, 0.1f, 3.0f);
        ImGui::SliderFloat("Ambient", &editor->m_ambient, 0.0f, 1.0f);
        ImGui::SliderFloat("Metallic", &editor->m_metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Roughness", &editor->m_roughness, 0.0f, 1.0f);
        ImGui::SliderFloat("Reflectance", &editor->m_reflectance, 0.0f, 1.0f);
        ImGui::Separator();

        ImGui::Text("Controls");
        std::string content;
        editor->m_translate ? content = "Translate" : content = "Scale";
        if(ImGui::Button(content.c_str()))
        {
            editor->m_translate = !editor->m_translate;
        }
        
        ImGui::End();
    }
}

void GlobalSettingsPanel::Close()
{
}
    
}
