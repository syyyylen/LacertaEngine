#pragma once

#include <unordered_set>
#include "InputListener.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API InputSystem
{
private:
    InputSystem();
    ~InputSystem();

public:
    static InputSystem* Get();
    static void Create();
    static void Release();

    void Update();
    void AddListener(InputListener* Listener);
    void RemoveListener(InputListener* Listener);

    void SetCursorPosition(const Point& pos);
    void ShowCursor(bool show);
    bool IsCursorVisible();

private:
    std::unordered_set<InputListener*> m_listenersSet;
    unsigned char m_keys_state[256] = {};
    unsigned char m_old_keys_state[256] = {};
    Point m_old_mouse_pos;
    bool m_first_time = true;

    static InputSystem* m_inputSystem;
    
};

}
