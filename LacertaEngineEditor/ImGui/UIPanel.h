#pragma once
#include "../LacertaEditor.h"

namespace LacertaEngineEditor
{

class UIPanel
{
public:
    UIPanel();
    virtual ~UIPanel();
    
    virtual void Start() = 0;
    virtual void Update() = 0;
    virtual void Close() = 0;
};
    
}
