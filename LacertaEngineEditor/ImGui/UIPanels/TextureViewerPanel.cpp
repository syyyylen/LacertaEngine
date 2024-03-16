#include "TextureViewerPanel.h"
#include "../UIRenderer.h"
#include "../imgui_src/imgui.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/Texture.h"

namespace LacertaEngineEditor
{
    
TextureViewerPanel::TextureViewerPanel()
{
}

TextureViewerPanel::~TextureViewerPanel()
{
}

void TextureViewerPanel::Start()
{
}

void TextureViewerPanel::Update()
{
    LacertaEditor* editor = UIRenderer::Get()->GetEditor();

    { 
        ImGui::Begin("Texture Viewer");
        
        ImGui::Text("Scene depth : ");

        auto SceneTextureRenderTarget = RHI::Get()->GetRenderTarget(1);
        if(SceneTextureRenderTarget != nullptr)
        {
            if(SceneTextureRenderTarget->RenderToTexture())
            {
                ImVec2 viewportSize = ImGui::GetContentRegionAvail();
                ImGui::Image(SceneTextureRenderTarget->GetDepthSRV(), viewportSize);
            }
        }
        
        // if(editor->m_irradianceTex)
        // {
        //     ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        //     ImGui::Image(editor->m_irradianceTex->GetTextureSRV(), viewportSize);
        // }

        ImGui::End();
    }
}

void TextureViewerPanel::Close()
{
}
    
}
