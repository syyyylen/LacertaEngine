#pragma once
#include "../UIPanel.h"

namespace LacertaEngineEditor
{

class DetailsPanel : public UIPanel
{
public:
    DetailsPanel();
    ~DetailsPanel();

    void Start() override;
    void Update() override;
    void Close() override;
};
    
}
