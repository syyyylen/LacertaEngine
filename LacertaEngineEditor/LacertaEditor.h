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

    // Scene Utilities
    Scene* GetActiveScene() { return m_activeScene; }
    
    void SaveActiveScene();
    Scene* LoadSceneFromFile(const wchar_t* filePath);
    
    bool HasSelectedGo() { return m_selectedObject != nullptr; }
    void SetSelectedGo(GameObject* selectedGo) { m_selectedObject = selectedGo; }
    GameObject* GetSelectedGo() { return m_selectedObject; }
    void DestroyGo(GameObject* goToDestroy);

    GameObject& AddMeshToScene(std::string name, const wchar_t* meshPath, Vector3 position = Vector3(0.0f, 0.0f, 0.0f), std::string shader = "MeshPBRShader");
    GameObject& AddPointLightToScene(Vector3 position = Vector3(0.0f, 0.0f, 0.0f), Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    GameObject& AddPBRSphereToScene(std::string name, Vector3 position, const wchar_t* albedo, const wchar_t* normal, const wchar_t* roughness, const wchar_t* metallic);
    // end Scene Utilities

    // Camera Utilities
    Matrix4x4 GetCameraMatrix() { return m_sceneCamera; }
    Matrix4x4 GetCameraProjectionMatrix() { return m_sceneCameraProjection; }
    // end Camera Utilities

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
    float m_moveSpeed = 31.0f;
    float m_inputDownScalar = 0.03f;

    float m_lightIntensity = 0.0f;
    float m_lightRotationX = 0.8f;
    float m_lightRotationY = 0.0f;
    float m_ambient = 1.0f;

    bool m_translate = true;
    bool m_directionalLightAutoRotate = false;
    float m_directionalLightAutoRotateScalar = 0.8f;

    Vector4 m_defaultColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

private:
    EditorWindow* m_editorWindow;
    Timer* m_globalTimer;
    Vector2 m_viewportCachedSize = Vector2(1920.0f, 1080.0f);

    unsigned long m_previousTickCount;
    float m_deltaTime;

    Scene* m_activeScene;
    GameObject* m_selectedObject;

    GameObject* m_skyBoxGo;

    // TODO handle scene Camera
    bool m_isMouseLocked = true;
    Matrix4x4 m_sceneCamera;
    Matrix4x4 m_sceneCameraProjection;
    float m_cameraForward;
    float m_cameraRight;
    float m_cameraRotationX;
    float m_cameraRotationY;
};
    
}
