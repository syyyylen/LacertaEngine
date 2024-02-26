#pragma once
#include "../UIPanel.h"

namespace LacertaEngineEditor
{
    
class TextureViewerPanel : public UIPanel
{
public:
    TextureViewerPanel();
    ~TextureViewerPanel();
    
    void Start() override;
    void Update() override;
    void Close() override;
};
    
}
