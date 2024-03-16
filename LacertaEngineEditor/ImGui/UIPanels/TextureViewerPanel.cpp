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

        auto ShadowMapRenderTarget = RHI::Get()->GetRenderTarget(editor->m_shadowMapRTidx);
        if(ShadowMapRenderTarget != nullptr)
        {
            if(ShadowMapRenderTarget->RenderToTexture())
            {
                ImVec2 viewportSize = ImGui::GetContentRegionAvail();
                ImGui::Image(ShadowMapRenderTarget->GetSRV(), viewportSize);
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
