#pragma once
#include "../Maths/Point.h"

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
    virtual void OnMouseMove(const Point& mousePosition) = 0;
    virtual void OnLeftMouseDown(const Point& mousePos) = 0;
    virtual void OnRightMouseDown(const Point& mousePos) = 0;
    virtual void OnLeftMouseUp(const Point& mousePos) = 0;
    virtual void OnRightMouseUp(const Point& mousePos) = 0;
};

}
