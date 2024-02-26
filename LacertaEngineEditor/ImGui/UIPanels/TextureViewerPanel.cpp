#include "TextureViewerPanel.h"
#include "../UIRenderer.h"
#include "../imgui_src/imgui.h"

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
        
        ImGui::Text("Irradiance tex : ");

        ImGui::End();
    }
}

void TextureViewerPanel::Close()
{
}
    
}
