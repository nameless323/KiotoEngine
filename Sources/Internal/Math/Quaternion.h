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
    Quaternion();
    Quaternion(const Vector3& axis, float32 angle);
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
    float32 LengthSqr() const;
    Quaternion Inverse() const;
    Quaternion InverseNonNorm() const;
    Quaternion Normalized() const;
    Quaternion& Normalize();
    Quaternion Rotation(const Vector3& axis, float32 angle) const;
    Vector3 RotateVector(const Vector3& v) const;
    Matrix4 ToMatrix() const;
    void SetFromMatrix(const Matrix4& m);
    void AxisAngle(const Vector3& axis, float32 angle);

    static float32 Dot(const Quaternion& q1, const Quaternion& q2);
    static Quaternion SLerp(const Quaternion& q1, const Quaternion& q2, float32 t);
    static Quaternion FromEuler(float32 x, float32 y, float32 z);
    static Vector3 ToEuler(const Quaternion& q);
    static Quaternion FromMatrix(const Matrix4& m);
    static Quaternion LookAt(const Vector3& sourcePos, const Vector3& targetPos, const Vector3& fwd);

    static Quaternion Identity;
};

inline Quaternion::Quaternion(const Quaternion& q)
    : v(q.v)
{}

inline Quaternion::Quaternion(float32 x_, float32 y_, float32 z_, float32 w_)
    : x(x_), y(y_), z(z_), w(w_)
{}

inline Quaternion::Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f)
{}

inline Quaternion::Quaternion(const Vector3& axis, float32 angle)
{
    AxisAngle(axis, angle);
}

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

inline Quaternion& Quaternion::operator*=(float32 f)
{
    x = x * f;
    y = y * f;
    z = z * f;
    w = w * f;
    return *this;
}

inline Quaternion operator* (Quaternion q, float32 f)
{
    q *= f;
    return q;
}


inline Quaternion Quaternion::Conjugate() const
{
    return { -x, -y, -z, w};
}

inline float32 Quaternion::Length() const
{
    return std::sqrt(LengthSqr());
}

inline float32 Quaternion::LengthSqr() const
{
    return x * x + y * y + z * z + w * w;
}

inline Quaternion Quaternion::Inverse() const
{
    assert(Math::IsFloatEqual(Length(), 1.0f));
    return Conjugate();
}

inline Quaternion Quaternion::InverseNonNorm() const
{
    return Conjugate() / LengthSqr();
}

inline Quaternion Quaternion::Normalized() const
{
    Quaternion q = *this;
    q /= Length();
    return q;
}

inline Quaternion& Quaternion::Normalize()
{
    float32 lenInv = 1.0f / Length();
    *this *= lenInv;
    return *this;
}

inline Quaternion Quaternion::Rotation(const Vector3& axis, float32 angle) const
{
    float32 halfAngle = angle * 0.5f;
    float32 cosHalfAngle = std::cos(halfAngle);
    float32 sinHalfAngle = std::sin(halfAngle);
    Vector3 vecPart = axis * sinHalfAngle;
    return { vecPart.x, vecPart.y, vecPart.z, cosHalfAngle };
}

inline Vector3 Quaternion::RotateVector(const Vector3& v) const
{
    Quaternion pureV = { v.x, v.y, v.z, 0.0f };
    Quaternion res = *this * pureV * Conjugate();
    return { res.x, res.y, res.z };
}

inline Matrix4 Quaternion::ToMatrix() const
{
    // [a_vorontcov] TODO: optimize squares.
    Matrix4 m
    {
        1.0f - 2.0f * y * y - 2.0f * z * z, 2.0f * x * y - 2.0f * w * z, 2.0f * x * z + 2.0f * w * y, 0.0f,
        2.0f * x * y + 2.0f * w * z, 1.0f - 2.0f * x * x - 2.0f * z * z, 2.0f * y * z - 2.0f * w * x, 0.0f,
        2.0f * x * z - 2.0f * w * y, 2.0f * y * z + 2.0f * w * x, 1.0f - 2.0f * x * x - 2.0f * y * y, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    return m;
}

inline void Quaternion::SetFromMatrix(const Matrix4& m)
{
    // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    float32 trace = m._00 + m._11 + m._22;
    if (trace > 0)
    {
        float32 s = 0.5f / std::sqrt(trace + 1.0f);
        w = 0.25f / s;
        x = (m._21 - m._12) * s;
        y = (m._02 - m._20) * s;
        z = (m._10 - m._01) * s;
        return;
    }

    if (m._00 > m._11 && m._00 > m._22)
    {
        float32 s = 2.0f * std::sqrt(1.0f + m._00 - m._11 - m._22);
        w = (m._21 - m._12) / s;
        x = 0.25f * s;
        y = (m._01 + m._10) / s;
        z = (m._02 + m._20) / s;
        return;
    }

    if (m._11 > m._22)
    {
        float32 s = 2.0f * std::sqrt(1.0f + m._11 - m._00 - m._22);
        w = (m._02 - m._20) / s;
        x = (m._01 + m._10) / s;
        y = 0.25f * s;
        z = (m._12 + m._21) / s;
        return;
    }

    float32 s = 2.0f * std::sqrt(1.0f + m._22 - m._00 - m._11);
    w = (m._10 - m._01) / s;
    x = (m._02 + m._20) / s;
    y = (m._12 + m._21) / s;
    z = 0.25f * s;
}

inline void Quaternion::AxisAngle(const Vector3& axis, float32 angle)
{
    float32 halfAngle = angle * 0.5f;
    float32 sinHAngle = std::sin(halfAngle);
    float32 cosHAngle = std::cos(halfAngle);
    w = cosHAngle;
    x = axis.x * sinHAngle;
    y = axis.y * sinHAngle;
    z = axis.z * sinHAngle;
}

inline float32 Quaternion::Dot(const Quaternion& q1, const Quaternion& q2)
{
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

inline Quaternion Quaternion::SLerp(const Quaternion& q1, const Quaternion& q2, float32 t)
{
    float32 dotSqr = Quaternion::Dot(q1, q2);
    float32 theta = std::acos(dotSqr);
    float32 sinThetaInv = 1.0f / std::sin(theta);
    float32 oneMinusT = 1.0f - t;
    float32 wa = std::sin(theta * oneMinusT) * sinThetaInv;
    float32 wb = std::sin(theta * t) * sinThetaInv;
    Quaternion q1_ = q1 * wa;
    Quaternion q2_ = q2 * wb;
    Quaternion res = { q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w };
    res.Normalize();
    return res;
}

inline Quaternion Quaternion::FromEuler(float32 x, float32 y, float32 z)
{
    float32 halfX = x * 0.5f;
    float32 halfY = y * 0.5f;
    float32 halfZ = z * 0.5f;
    float32 cx = std::cos(halfX);
    float32 sx = std::sin(halfX);
    float32 cy = std::cos(halfY);
    float32 sy = std::sin(halfY);
    float32 cz = std::cos(halfZ);
    float32 sz = std::sin(halfZ);

    Quaternion q;
    q.w = cx * cy * cz + sx * sy * sz;
    q.x = sx * cy * cz - cx * sy * sz;
    q.y = cx * sy * cz + sx * cy * sz;
    q.z = cx * cy * sz - sx * sy * cz;

    return q;
}

inline Vector3 Quaternion::ToEuler(const Quaternion& q)
{
    float32 sinrCosp = 2.0f * (q.w * q.x + q.y * q.z);
    float32 cosrCosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    float32 x = std::atan2(sinrCosp, cosrCosp);

    float32 y = 0.0f;
    float32 sinp = 2.0f * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1.0f)
        y = std::copysign(Math::PIOverTwo, sinp);
    else
        y = std::asin(sinp);

    float32 sinyCosp = 2.0f * (q.w * q.z + q.x * q.y);
    float32 cosyCosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    float32 z = std::atan2(sinyCosp, cosyCosp);

    return { x, y, z };
}

inline Quaternion Quaternion::FromMatrix(const Matrix4& m)
{
    Quaternion q;
    q.SetFromMatrix(m);
    return q;
}

inline Quaternion Quaternion::LookAt(const Vector3& sourcePos, const Vector3& targetPos, const Vector3& fwd)
{
    Vector3 trgtVec = targetPos - sourcePos;
    trgtVec.Normalize();
    Vector3 up = Vector3::Cross(fwd, trgtVec);
    up.Normalize();
    float32 dot = Vector3::Dot(trgtVec, fwd);
    if (Math::IsFloatEqual(dot, -1.0f))
        return Quaternion(Vector3::Up, Math::DegToRad(180.0f));
    if (Math::IsFloatEqual(dot, 1.0f))
        return Quaternion::Identity;
    float32 angle = std::acos(dot);
    return Quaternion(up, angle);
}

inline Quaternion Quaternion::Identity = { 0.0f, 0.0f, 0.0f, 1.0f };

}