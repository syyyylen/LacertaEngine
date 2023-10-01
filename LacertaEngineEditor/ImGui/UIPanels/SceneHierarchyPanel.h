#pragma once
#include "../UIPanel.h"

namespace LacertaEngineEditor
{

class SceneHierarchyPanel : public UIPanel
{
public:
    SceneHierarchyPanel();
    ~SceneHierarchyPanel();

    void Start() override;
    void Update() override;
    void Close() override;
};
    
}
