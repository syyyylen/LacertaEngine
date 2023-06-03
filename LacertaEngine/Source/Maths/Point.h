#pragma once
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Point
{
public:
    Point() :X(0), Y(0)
    {
    }
    
    Point(int x,int y):X(x),Y(y)
    {
    }
    
    Point(const Point & point) :X(point.X), Y(point.Y)
    {
    }
    
    ~Point()
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
