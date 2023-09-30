#pragma once
#include <LacertaEngine.h>
#include "EditorWindow/EditorWindow.h"

namespace LacertaEngineEditor
{

using namespace LacertaEngine; // This is making the code less explicit. Might remove it if it's too confusing. 

/**
 * \brief Handle core logic for the Editor, such as Update Loops for entities etc...
 * 
 */
class LacertaEditor : public InputListener
{
public:
    LacertaEditor();
    ~LacertaEditor();

    void Start();
    void Update();
    void Quit();

    void Close();
    bool IsRunning();

    void SetViewportCachedSize(Vector2 newSize) { m_viewportCachedSize = newSize; }
    Vector2 GetViewportCachedSize() { return m_viewportCachedSize; }
    EditorWindow* GetEditorWindow();
    Scene* GetActiveScene() { return m_activeScene; }

    // InputListener interface
    void OnKeyDown(int key) override;
    void OnKeyUp(int key) override;
    void OnMouseMove(const Vector2& mousePosition) override;
    virtual void OnLeftMouseDown(const Vector2& mousePos) override;
    virtual void OnRightMouseDown(const Vector2& mousePos) override;
    virtual void OnLeftMouseUp(const Vector2& mousePos) override;
    virtual void OnRightMouseUp(const Vector2& mousePos) override;
    // end InputListener interface

private:
    EditorWindow* m_editorWindow;
    Timer* m_globalTimer;
    Vector2 m_viewportCachedSize = Vector2(1920.0f, 1080.0f);

    unsigned long m_previousTickCount;
    float m_deltaTime;

    Scene* m_activeScene;

    // TODO handle scene Camera
    bool m_isMouseLocked = true;
    Matrix4x4 m_sceneCamera;
    float m_cameraForward;
    float m_cameraRight;
    float m_cameraRotationX;
    float m_cameraRotationY;
};
    
}
