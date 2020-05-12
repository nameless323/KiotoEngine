#pragma once

#include "Core/CoreTypes.h"
#include "Math/MathHelpers.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"

namespace Kioto
{
class Quaternion
{
public:
    union
    {
        struct
        {
            float32 w; float32 x; float32 y; float32 z;
        };
        Vector4 v;
        float32 data[4];
    };
    Quaternion(float32 x, float32 y, float32 z, float32 w);
    Quaternion(const Quaternion& q);
    Quaternion& operator= (const Quaternion& q);

    bool operator== (const Quaternion& q) const;
    bool operator!= (const Quaternion& q) const;
    Quaternion& operator*=(const Quaternion& q);
    Quaternion& operator*=(float32 f);
    Quaternion& operator/=(float32 f);
    Quaternion Conjugate() const;
    float32 Length() const;
    Quaternion Inverse() const;
    Quaternion InverseNonNorm() const;
};

inline Quaternion::Quaternion(const Quaternion& q)
    : v(q.v)
{}

inline Quaternion::Quaternion(float32 x_, float32 y_, float32 z_, float32 w_)
    : x(x_), y(y_), z(z_), w(w_)
{}

inline Quaternion& Quaternion::operator= (const Quaternion& q)
{
    v = q.v;
    return *this;
}

inline bool Quaternion::operator== (const Quaternion& q) const
{
    return v == q.v;
}

inline bool Quaternion::operator!= (const Quaternion& q) const
{
    return !(*this == q);
}

inline Quaternion& Quaternion::operator*= (const Quaternion& q)
{
    Quaternion t = *this;
    w = t.w * q.w - t.x * q.x - t.y * q.y - t.z * q.z;
    x = t.w * q.x + t.x * q.w + t.y * q.z - t.z * q.y;
    y = t.w * q.y - t.x * q.z + t.y * q.w + t.z * q.x;
    z = t.w * q.z + t.x * q.y - t.y * q.x + t.z * q.w;

    return *this;
}

inline Quaternion operator* (Quaternion q1, const Quaternion& q2)
{
    q1 *= q2;
    return q1;
}

inline Quaternion operator* (Quaternion q1, const Quaternion& q2)
{
    q1 *= q2;
    return q1;
}

inline Quaternion& Quaternion::operator/= (float32 f)
{
    float32 fInv = 1.0f;
    x = x * fInv;
    y = y * fInv;
    z = z * fInv;
    w = w * fInv;
    return *this;
}

inline Quaternion operator/ (Quaternion q, float32 f)
{
    q /= f;
    return q;
}


inline Quaternion Quaternion::Conjugate() const
{
    return { -x, -y, -z, w};
}

float32 Quaternion::Length() const
{
    return std::sqrt(x * x + y * y + z * z + w * w);
}

Quaternion Quaternion::Inverse() const
{
    assert(Math::IsFloatEqual(Length(), 1.0f));
    return Conjugate();
}

Quaternion Quaternion::InverseNonNorm() const
{
    return Conjugate() / Length();
}

}