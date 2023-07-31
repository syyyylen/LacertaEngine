#pragma once
#include "../Maths/Vector2.h"

namespace LacertaEngine
{

class LACERTAENGINE_API InputListener
{
public:
    InputListener()
    {
    }

    ~InputListener()
    {
    }

    // Keyboard pure virtual callbacks
    virtual void OnKeyDown(int key) = 0;
    virtual void OnKeyUp(int key) = 0;

    // Mouse pure virtual callbacks
    virtual void OnMouseMove(const Vector2& mousePosition) = 0;
    virtual void OnLeftMouseDown(const Vector2& mousePos) = 0;
    virtual void OnRightMouseDown(const Vector2& mousePos) = 0;
    virtual void OnLeftMouseUp(const Vector2& mousePos) = 0;
    virtual void OnRightMouseUp(const Vector2& mousePos) = 0;
};

}
