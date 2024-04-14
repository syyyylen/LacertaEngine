﻿#include "SceneHierarchyPanel.h"

#include "../UIRenderer.h"
#include "../imgui_src/imgui.h"

namespace LacertaEngineEditor
{

SceneHierarchyPanel::SceneHierarchyPanel()
{
}

SceneHierarchyPanel::~SceneHierarchyPanel()
{
}

void SceneHierarchyPanel::Start()
{
}

void SceneHierarchyPanel::Update()
{
    LacertaEditor* editor = UIRenderer::Get()->GetEditor();
    Scene* activeScene = editor->GetActiveScene();

    ImGui::Begin("SceneHierarchy");

    if(ImGui::BeginListBox("Game Objects"))
    {
        for(auto go : activeScene->m_gameObjects)
        {
            bool isSelected = false;
            if(editor->HasSelectedGo())
            {
               if(editor->GetSelectedGo()->GetName() == go->GetName())
                   isSelected = true;
            }
            
            if(ImGui::Selectable(go->GetName().c_str(), isSelected))
                editor->SetSelectedGo(go);
        }

        ImGui::EndListBox();
    }
    ImGui::Separator();

    if(ImGui::Button("Create Sphere"))
    {
        editor->SetSelectedGo(&editor->ImportMeshToScene("Sphere", "Assets/Meshes/sphere.gltf"));
    }
    if(ImGui::Button("Create Cube"))
    {
        editor->SetSelectedGo(&editor->ImportMeshToScene("Cube", "Assets/Meshes/cube.obj"));
    }
    if(ImGui::Button("Create Light"))
    {
        editor->SetSelectedGo(&editor->AddPointLightToScene());
    }
    ImGui::Separator();

    if(ImGui::Button("Delete"))
    {
        if(editor->HasSelectedGo())
        {
            editor->DestroyGo(editor->GetSelectedGo());
        }
    }
    
    ImGui::End();
}

void SceneHierarchyPanel::Close()
{
}
    
}
