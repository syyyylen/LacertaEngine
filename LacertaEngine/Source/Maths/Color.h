#pragma once
#include "../Core.h"

namespace LacertaEngine
{
    
    class LACERTAENGINE_API Color
    {
    public:
        Color() :R(0), G(0), B(0), A(0)
        {
        }
    
        Color(float r,float g, float b, float a):R(r), G(g), B(b), A(a)
        {
        }
    
        Color(const Color& color) :R(color.R), G(color.G), B(color.B), A(color.A)
        {
        }
    
        ~Color()
        {
        }

        std::string ToString() const
        {
            return "(" + std::to_string(R) +
                    ", " +
                    std::to_string(G) +
                    ", " +
                    std::to_string(B) +
                    ", " +
                    std::to_string(A) +
                    ")";
        }
    
    public:
        float R, G, B, A;
    };

}