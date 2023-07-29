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
    
    Vector2(int x,int y):X(x),Y(y)
    {
    }
    
    Vector2(const Vector2 & point) :X(point.X), Y(point.Y)
    {
    }

    Vector2 operator *(float num)
    {
        return Vector2(X*num, Y*num);
    }

    Vector2 operator +(Vector2 vec)
    {
        return Vector2(X + vec.X, Y + vec.Y);
    }
    
    ~Vector2()
    {
    }

    std::string ToString() const
    {
        return "(" + std::to_string(X) + ", " + std::to_string(Y) + ")";
    }
    
public:
    int X = 0, Y = 0;
};

}
