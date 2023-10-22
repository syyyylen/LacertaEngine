#pragma once
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Vector2
{
public:
    Vector2() :X(0), Y(0)
    {
    }
    
    Vector2(float x,float y):X(x),Y(y)
    {
    }
    
    Vector2(const Vector2 & point) :X(point.X), Y(point.Y)
    {
    }

    Vector2 operator *(float num) const
    {
        return Vector2(X*num, Y*num);
    }

    Vector2 operator +(const Vector2& vec) const
    {
        return Vector2(X + vec.X, Y + vec.Y);
    }

    Vector2 operator -(const Vector2& vec) const
    {
        return Vector2(X - vec.X, Y - vec.Y);
    }
    
    ~Vector2()
    {
    }

    std::string ToString() const
    {
        return "(" + std::to_string(X) + ", " + std::to_string(Y) + ")";
    }
    
public:
    float X = 0, Y = 0;
};

}
