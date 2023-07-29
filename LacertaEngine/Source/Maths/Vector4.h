#pragma once
#include "Vector3.h"
#include "../Core.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Vector4
{
public:
    Vector4() : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f)
    {
    }

    Vector4(float _x, float _y, float _z, float _w) : X(_x), Y(_y), Z(_z), W(_w)
    {
    }

    Vector4(const Vector4& v) : X(v.X), Y(v.Y), Z(v.Z), W(v.W)
    {
    }

    Vector4(const Vector3& v) : X(v.X), Y(v.Y), Z(v.Z), W(1.0)
    {
    }


    ~Vector4()
    {
    }

    void Cross(Vector4 &v1, Vector4 &v2, Vector4 &v3)
    {
        this->X = v1.Y * (v2.Z * v3.W - v3.Z * v2.W) - v1.Z * (v2.Y * v3.W - v3.Y * v2.W) + v1.W * (v2.Y * v3.Z - v2.Z *v3.Y);
        this->Y = -(v1.X * (v2.Z * v3.W - v3.Z * v2.W) - v1.Z * (v2.X * v3.W - v3.X * v2.W) + v1.W * (v2.X * v3.Z - v3.X * v2.Z));
        this->Z = v1.X * (v2.Y * v3.W - v3.Y * v2.W) - v1.Y * (v2.X *v3.W - v3.X * v2.W) + v1.W * (v2.X * v3.Y - v3.X * v2.Y);
        this->W = -(v1.X * (v2.Y * v3.Z - v3.Y * v2.Z) - v1.Y * (v2.X * v3.Z - v3.X *v2.Z) + v1.Z * (v2.X * v3.Y - v3.X * v2.Y));
    }

    std::string ToString() const
    {
        return "(" + std::to_string(X) +
                ", " +
                std::to_string(Y) +
                ", " +
                std::to_string(Z) +
                ", " +
                std::to_string(W) +
                ")";
    }

public:
    float X, Y, Z, W;
};

}