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

    bool HasSelectedGo() { return m_selectedObject != nullptr; }
    void SetSelectedGo(GameObject* selectedGo) { m_selectedObject = selectedGo; }
    GameObject* GetSelectedGo() { return m_selectedObject; }
    void DestroyGo(GameObject* goToDestroy);

    // InputListener interface
    void OnKeyDown(int key) override;
    void OnKeyUp(int key) override;
    void OnMouseMove(const Vector2& mousePosition) override;
    virtual void OnLeftMouseDown(const Vector2& mousePos) override;
    virtual void OnRightMouseDown(const Vector2& mousePos) override;
    virtual void OnLeftMouseUp(const Vector2& mousePos) override;
    virtual void OnRightMouseUp(const Vector2& mousePos) override;
    // end InputListener interface
    
    // Temporary constants, exposed for UI panels
public:
    float m_mouseSensivity = 3.0f;
    float m_moveSpeed = 12.5f;
    float m_inputDownScalar = 0.03f;

    float m_lightRotation = 0.0f;
    float m_ambient = 0.1f;
    float m_diffuse = 1.0f;
    float m_specular = 1.0f;
    float m_shininess = 10.0f;

private:
    EditorWindow* m_editorWindow;
    Timer* m_globalTimer;
    Vector2 m_viewportCachedSize = Vector2(1920.0f, 1080.0f);

    unsigned long m_previousTickCount;
    float m_deltaTime;

    Scene* m_activeScene;
    GameObject* m_selectedObject;

    // TODO handle scene Camera
    bool m_isMouseLocked = true;
    Matrix4x4 m_sceneCamera;
    float m_cameraForward;
    float m_cameraRight;
    float m_cameraRotationX;
    float m_cameraRotationY;
};
    
}
