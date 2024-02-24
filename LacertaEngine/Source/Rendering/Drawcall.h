#pragma once

#include "Bindable.h"
#include "Drawable.h"
#include "../Core.h"
#include "Renderer.h"
#include "ScenePassLayouts.h" // include for rest of files
#include "ConstantBuffer.h" // include for rest of files

namespace LacertaEngine
{

enum DrawableLayout
{
    SceneMesh
};

class Shader;
    
class LACERTAENGINE_API Drawcall
{
public:
    Drawcall(std::string shaderName, Drawable* drawable, std::vector<Bindable*> bindables);
    virtual ~Drawcall();

    void PreparePass(Renderer* renderer);
    void Pass(Renderer* renderer);
    Drawable* GetDrawable() { return m_drawable; }
    std::string GetShaderName() { return m_shaderName; }

private:
    std::string m_shaderName;
    Shader* m_shader;
    Drawable* m_drawable;
    std::vector<Bindable*> m_bindables;
};

}
