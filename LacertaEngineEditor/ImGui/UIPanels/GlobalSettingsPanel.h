#pragma once
#include "../UIPanel.h"

namespace LacertaEngineEditor
{
    
class GlobalSettingsPanel : public UIPanel
{
public:
    GlobalSettingsPanel();
    ~GlobalSettingsPanel();
    
    void Start() override;
    void Update() override;
    void Close() override;
};

}
