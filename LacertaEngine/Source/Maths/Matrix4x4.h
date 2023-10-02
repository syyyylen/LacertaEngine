#pragma once

#include "../Core.h"
#include "Vector3.h"
#include "Vector4.h"

namespace LacertaEngine
{
    
class LACERTAENGINE_API Matrix4x4
{
public:
    Matrix4x4() 
    {
    }

    ~Matrix4x4()
    {
    }

    void SetIdentity()
    {
        memset(m_mat, 0, sizeof(float) * 16);
        m_mat[0][0] = 1.0f;
        m_mat[1][1] = 1.0f;
        m_mat[2][2] = 1.0f;
        m_mat[3][3] = 1.0f;
    }

    void SetMatrix(const Matrix4x4& matrix)
    {
        ::memcpy(m_mat, matrix.m_mat, sizeof(float) * 16);
    }

    void SetTranslation(const Vector3& translation)
    {
        m_mat[3][0] = translation.X;
        m_mat[3][1] = translation.Y;
        m_mat[3][2] = translation.Z;
    }

    void SetScale(const Vector3& scale)
    {
        m_mat[0][0] = scale.X;
        m_mat[1][1] = scale.Y;
        m_mat[2][2] = scale.Z;
    }

    void SetRotationX(float x)
    {
        m_mat[1][1] = cos(x);
        m_mat[1][2] = sin(x);
        m_mat[2][1] = -sin(x);
        m_mat[2][2] = cos(x);
    }

    void SetRotationY(float y)
    {
        m_mat[0][0] = cos(y);
        m_mat[0][2] = -sin(y);
        m_mat[2][0] = sin(y);
        m_mat[2][2] = cos(y);
    }

    void SetRotationZ(float z)
    {
        m_mat[0][0] = cos(z);
        m_mat[0][1] = sin(z);
        m_mat[1][0] = -sin(z);
        m_mat[1][1] = cos(z);
    }

    void SetPerspectiveFovLH(float fov, float aspect, float znear, float zfar)
    {
        float yscale = 1.0f / tan(fov / 2.0f);
        float xscale = yscale / aspect;
        m_mat[0][0] = xscale;
        m_mat[1][1] = yscale;
        m_mat[2][2] = zfar / (zfar - znear);
        m_mat[2][3] = 1.0f;
        m_mat[3][2] = (-znear*zfar)/ (zfar - znear);
    }

    void SetOrthoLH(float width, float height, float nearPlane, float farPlane)
    {
        SetIdentity();
        m_mat[0][0] = 2.0f / width;
        m_mat[1][1] = 2.0f / height;
        m_mat[2][2] = 1.0f / (farPlane - nearPlane);
        m_mat[3][2] = -(nearPlane / (farPlane - nearPlane));
    }

    float GetDeterminant()
    {
        Vector4 minor, v1, v2, v3;
        float det;

        v1 = Vector4(this->m_mat[0][0], this->m_mat[1][0], this->m_mat[2][0], this->m_mat[3][0]);
        v2 = Vector4(this->m_mat[0][1], this->m_mat[1][1], this->m_mat[2][1], this->m_mat[3][1]);
        v3 = Vector4(this->m_mat[0][2], this->m_mat[1][2], this->m_mat[2][2], this->m_mat[3][2]);


        minor.Cross(v1, v2, v3);
        det = -(this->m_mat[0][3] * minor.X + this->m_mat[1][3] * minor.Y + this->m_mat[2][3] * minor.Z + this->m_mat[3][3] * minor.W);
        return det;
    }


    void Inverse()
    {
        int a, i, j;
        Matrix4x4 out;
        Vector4 v, vec[3];
        float det = 0.0f;

        det = this->GetDeterminant();
        if (!det) return;
        for (i = 0; i<4; i++)
        {
            for (j = 0; j<4; j++)
            {
                if (j != i)
                {
                    a = j;
                    if (j > i) a = a - 1;
                    vec[a].X = (this->m_mat[j][0]);
                    vec[a].Y = (this->m_mat[j][1]);
                    vec[a].Z = (this->m_mat[j][2]);
                    vec[a].W = (this->m_mat[j][3]);
                }
            }
            v.Cross(vec[0], vec[1], vec[2]);

            out.m_mat[0][i] = pow(-1.0f, i) * v.X / det;
            out.m_mat[1][i] = pow(-1.0f, i) * v.Y / det;
            out.m_mat[2][i] = pow(-1.0f, i) * v.Z / det;
            out.m_mat[3][i] = pow(-1.0f, i) * v.W / det;
        }

        this->SetMatrix(out);
    }

    Vector3 GetZDirection() 
    {
        return Vector3(m_mat[2][0], m_mat[2][1], m_mat[2][2]);
    }
    Vector3 GetXDirection() 
    {
        return Vector3(m_mat[0][0], m_mat[0][1], m_mat[0][2]);
    }
    Vector3 GetTranslation() 
    {
        return Vector3(m_mat[3][0], m_mat[3][1], m_mat[3][2]);
    }

    Vector3 GetScale() 
    {
        return Vector3(
            Vector3(m_mat[0][0], m_mat[0][1], m_mat[0][2]).Length(),
            Vector3(m_mat[1][0], m_mat[1][1], m_mat[1][2]).Length(),
            Vector3(m_mat[2][0], m_mat[2][1], m_mat[2][2]).Length()
        );
    }

    // Matrix4x4 operator*(const Matrix4x4& other)
    // {
    //     Matrix4x4 result;
    //
    //     for (int i = 0; i < 4; ++i) {
    //         for (int j = 0; j < 4; ++j) {
    //             result.m_mat[i][j] = 0.0f;
    //             for (int k = 0; k < 4; ++k) {
    //                 result.m_mat[i][j] += m_mat[i][k] * other.m_mat[k][j];
    //             }
    //         }
    //     }
    //
    //     return result;
    // }

    void operator *=(const Matrix4x4& matrix)
    {
        Matrix4x4 out;
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                out.m_mat[i][j] =
                    m_mat[i][0] * matrix.m_mat[0][j] + m_mat[i][1] * matrix.m_mat[1][j] +
                    m_mat[i][2] * matrix.m_mat[2][j] + m_mat[i][3] * matrix.m_mat[3][j];
            }
        }
        memcpy(m_mat, out.m_mat, sizeof(float) * 16);
    }

    Vector4 operator*(const Vector4& vector) const
    {
        return Vector4(
            m_mat[0][0] * vector.X + m_mat[0][1] * vector.Y + m_mat[0][2] * vector.Z + m_mat[0][3] * vector.W,
            m_mat[1][0] * vector.X + m_mat[1][1] * vector.Y + m_mat[1][2] * vector.Z + m_mat[1][3] * vector.W,
            m_mat[2][0] * vector.X + m_mat[2][1] * vector.Y + m_mat[2][2] * vector.Z + m_mat[2][3] * vector.W,
            m_mat[3][0] * vector.X + m_mat[3][1] * vector.Y + m_mat[3][2] * vector.Z + m_mat[3][3] * vector.W
        );
    }

    float* ToFloatPtr() { return m_mat[0]; }

private:
    float m_mat[4][4] = {};

private:
    friend class Vector3;
    
};
    
}
