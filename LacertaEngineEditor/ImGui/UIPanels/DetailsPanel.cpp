#include "DetailsPanel.h"

#include "../UIRenderer.h"
#include "../imgui_src/imgui.h"

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
