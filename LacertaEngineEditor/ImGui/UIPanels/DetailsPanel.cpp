#include "DetailsPanel.h"

#include "../UIRenderer.h"
#include "../imgui_src/imgui.h"
#include "ECS/Components/PointLightComponent.h"

namespace LacertaEngineEditor
{
    
DetailsPanel::DetailsPanel()
{
}

DetailsPanel::~DetailsPanel()
{
}

void DetailsPanel::Start()
{
}

void DetailsPanel::Update()
{
    LacertaEditor* editor = UIRenderer::Get()->GetEditor();

    ImGui::Begin("Details");

    if(editor->HasSelectedGo())
    {
        GameObject* selectedGo = editor->GetSelectedGo();
        ImGui::Text(selectedGo->GetName().c_str());
        
        ImGui::Separator();

        if(selectedGo->HasComponent<TransformComponent>())
        {
            ImGui::Text("Transform");
            ImGui::Spacing();
            
            TransformComponent& tfComp = selectedGo->GetComponent<TransformComponent>();

            // Position
            float pos[3] = { tfComp.Position().X, tfComp.Position().Y, tfComp.Position().Z };
            ImGui::InputFloat3("Position", pos);
            tfComp.SetPosition(Vector3(pos[0], pos[1], pos[2]));

            // Scale
            float scale[3] = { tfComp.Scale().X, tfComp.Scale().Y, tfComp.Scale().Z };
            ImGui::InputFloat3("Scale", scale);
            tfComp.SetScale(Vector3(scale[0], scale[1], scale[2]));
        }

        ImGui::Separator();

        if(selectedGo->HasComponent<MeshComponent>())
        {
            ImGui::Text("Mesh");
            ImGui::Spacing();
            
            MeshComponent& meshComp = selectedGo->GetComponent<MeshComponent>();

            if(meshComp.GetMaterial() != nullptr)
            {
                Material* mat = meshComp.GetMaterial();
                
                ImGui::Text("Material Shader : %s", mat->GetShader().c_str());
                ImGui::Spacing();

                MatLightProperties lightProperties = mat->GetMatLightProperties();

                // TODO remove old phong uniforms
                // ImGui::SliderFloat("Diffuse", &lightProperties.DiffuseIntensity, 0.0f, 5.0f);
                // ImGui::SliderFloat("Specular", &lightProperties.SpecularIntensity, 0.0f, 5.0f);
                // ImGui::SliderFloat("Shininess", &lightProperties.Shininess, 0.0f, 1.0f);
                ImGui::SliderFloat("Metallic", &lightProperties.Metallic, 0.0f, 1.0f);
                ImGui::SliderFloat("Roughness", &lightProperties.Roughness, 0.0f, 1.0f);

                mat->SetMatLightProperties(lightProperties);
            }
        }

        ImGui::Separator();

        if(selectedGo->HasComponent<PointLightComponent>())
        {
            ImGui::Text("Point Light");
            ImGui::Spacing();
            
            PointLightComponent& lightComp = selectedGo->GetComponent<PointLightComponent>();

            float color[4] = { lightComp.GetColor().X, lightComp.GetColor().Y, lightComp.GetColor().Z, lightComp.GetColor().W };
            ImGui::ColorEdit4("Color", color);
            lightComp.SetColor(Vector4(color[0], color[1], color[2], color[3]));

            float constant = lightComp.GetConstantAttenuation();
            float linear = lightComp.GetLinearAttenuation();
            float quadratic = lightComp.GetQuadraticAttenuation();

            ImGui::Text("Attenuation : ");
            ImGui::SliderFloat("Constant", &constant, 0.0f, 1.0f);
            ImGui::SliderFloat("Linear", &linear, 0.0f, 1.0f);
            ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 1.0f);
            lightComp.SetAttenuation(constant, linear, quadratic);
        }

        ImGui::Separator();
    }
    else
    {
        ImGui::Text("No selected object");
    }
    
    ImGui::End();
}

void DetailsPanel::Close()
{
}
    
}
