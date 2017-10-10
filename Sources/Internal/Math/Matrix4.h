//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#if _WIN32 || _WIN64
#include <DirectXMath.h>
#endif

#include "Core/CoreTypes.h"
#include "Math/MathHelpers.h"
#include "Math/Vector4.h"

namespace Kioto
{
///
/// We use left handed coordinate system. Translation lies in the 4th row. Vector is a row matrix, so only left multiplication is allowed (vector * matrix).
/// Layout of ToWorldMatrix is
/// |  right.x,        right.y,       right.z,     0.0 |
/// |   up.x,            up.y,         up.z,       0.0 |
/// |   fwd.x,          fwd.y,         fwd.z,      0.0 |
/// | translation.x, translation.y, translation.z, 1.0 |
///
template <typename T>
class Matrix4_
{
public:
    union
    {
        Vector4_<T> r[4];
        struct
        {
            T _00, _01, _02, _03;
            T _10, _11, _12, _13;
            T _20, _21, _22, _23;
            T _30, _31, _32, _33;
        };
        T m[4][4];
        T data[16];
    };

    Matrix4_();
    Matrix4_(const Matrix4_<T>& other);
    Matrix4_(const Vector4_<T>& r0, const Vector4_<T>& r1, const Vector4_<T>& r2, const Vector4_<T>& r3);
    Matrix4_(
        T m_00, T m_01, T m_02, T m_03,
        T m_10, T m_11, T m_12, T m_13,
        T m_20, T m_21, T m_22, T m_23,
        T m_30, T m_31, T m_32, T m_33
        );

    Matrix4_<T>& operator= (const Matrix4_<T>& other);

    ///
    /// Get matrix determinant.
    ///
    float32 Determinant() const;
    ///
    /// Transpose matrix.
    ///
    void Transpose();
    ///
    /// Transpose matrix.
    ///
    Matrix4_<T> Tranposed() const;
    ///
    /// Inverse of the matrix. return false if inverse is impossible.
    ///
    bool Inversed(Matrix4_<T>& out) const;

    T& operator()(int32 row, int32 col);
    const T& operator()(int32 row, int32 col) const;

    ///
    /// Get translation from the matrix.
    ///
    Vector3_<T> GetTranslation() const;
    ///
    /// Set translation vector to the matrix.
    ///
    void SetTranslation(const Vector3_<T>& t);
    ///
    /// Get scale from the matrix.
    ///
    Vector3_<T> GetScale() const;

    Matrix4_<T>& operator*= (const Matrix4_<T>& m);

    ///
    /// Build rotation matrix. axs - axis of rotation. angle - angle in radians.
    ///
    static Matrix4_<T> BuildRotation(const Vector3_<T>& axs, float32 angle);
    ///
    /// Build rotation matrix around x axis. angle - angle in radians.
    ///
    static Matrix4_<T> BuildRotationX(float32 angle);
    ///
    /// Build rotation matrix around y axis. angle - angle in radians.
    ///
    static Matrix4_<T> BuildRotationY(float32 angle);
    ///
    /// Build rotation matrix around z axis. angle - angle in radians.
    ///
    static Matrix4_<T> BuildRotationZ(float32 angle);
    ///
    /// Build translation matrix. t - translation.
    ///
    static Matrix4_<T> BuildTranslation(const Vector3_<T>& t);
    ///
    /// Build scale matrix. s - scale.
    ///
    static Matrix4_<T> BuildScale(const Vector3_<T>& s);
    ///
    /// Build look at matrix (Left handed coordinate system). position - position, target - look target, up - world up.
    ///
    static Matrix4_<T> BuildLookAt(const Vector3& position, const Vector3& target, const Vector3& up);
    ///
    /// Build perspective projection matrix (Left handed coordinate system). fovY - field of view.
    ///
    static Matrix4_<T> BuildProjectionFov(float32 fovY, float32 aspect, float32 zNear, float32 zFar);
    ///
    /// Build orthographic projection matrix (Left handed coordinate system). fovY - field of view.
    ///
    static Matrix4_<T> BuildOrtho(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);

    static const Matrix4_<T>& Identity();
};

template <typename T>
inline Matrix4_<T> operator* (Matrix4_<T> m1, const Matrix4_<T>& m2)
{
    m1 *= m2;
    return m1;
}

template <typename T>
inline Vector4_<T> operator* (const Vector4_<T>& v, const Matrix4_<T>& m) // [a_vorontsov] Yep, it will be more obvious if it method will be in Vector4.h, but I'm too lazy to handle cross headers.
{
    return {
                v.x * m._00 + v.y * m._10 + v.z * m._20 + v.w * m._30,
                v.x * m._01 + v.y * m._11 + v.z * m._21 + v.w * m._31,
                v.x * m._02 + v.y * m._12 + v.z * m._22 + v.w * m._32,
                v.x * m._03 + v.y * m._13 + v.z * m._23 + v.w * m._33
            };
}

template <typename T>
Matrix4_<T>::Matrix4_()
{
    memset(data, 0, sizeof(data));
}

template <typename T>
Matrix4_<T>::Matrix4_(const Matrix4_<T>& other)
    : _00(other._00), _01(other._01), _02(other._02), _03(other._03),
      _10(other._10), _11(other._11), _12(other._12), _13(other._13),
      _20(other._20), _21(other._21), _22(other._22), _23(other._23),
      _30(other._30), _31(other._31), _32(other._32), _33(other._33)
{
}

template <typename T>
Matrix4_<T>::Matrix4_(const Vector4_<T>& r0, const Vector4_<T>& r1, const Vector4_<T>& r2, const Vector4_<T>& r3)
{
    r[0] = r0; r[1] = r1; r[2] = r2; r[3] = r3;
}

template <typename T>
Matrix4_<T>::Matrix4_(
    T m_00, T m_01, T m_02, T m_03,
    T m_10, T m_11, T m_12, T m_13,
    T m_20, T m_21, T m_22, T m_23,
    T m_30, T m_31, T m_32, T m_33
    )
    : _00(m_00), _01(m_01), _02(m_02), _03(m_03),
      _10(m_10), _11(m_11), _12(m_12), _13(m_13),
      _20(m_20), _21(m_21), _22(m_22), _23(m_23),
      _30(m_30), _31(m_31), _32(m_32), _33(m_33)
{
}

template <typename T>
Matrix4_<T>& Matrix4_<T>::operator=(const Matrix4_<T>& other)
{
    _00 = other._00; _01 = other._01; _02 = other._02; _03 = other._03; // [a_vorontsov] memcpy maybe?
    _10 = other._10; _11 = other._11; _12 = other._12; _13 = other._13;
    _20 = other._20; _21 = other._21; _22 = other._22; _23 = other._23;
    _30 = other._30; _31 = other._31; _32 = other._32; _33 = other._33;
    return *this;
}

template <typename T>
inline float32 Kioto::Matrix4_<T>::Determinant() const
{
    return (_00 * _11 - _01 * _10) * (_22 * _33 - _23 * _32) - (_00 * _12 - _02 * _10) * (_21 * _33 - _23 * _31)
         + (_00 * _13 - _03 * _10) * (_21 * _32 - _22 * _31) + (_01 * _12 - _02 * _11) * (_20 * _33 - _23 * _30)
         - (_01 * _13 - _03 * _11) * (_20 * _32 - _22 * _30) + (_02 * _13 - _03 * _12) * (_20 * _31 - _21 * _30);
}

template <typename T>
void Matrix4_<T>::Transpose()
{
    Matrix4_<T> m(
        _00, _10, _20, _30,
        _01, _11, _21, _31,
        _02, _12, _22, _32,
        _03, _13, _23, _33);
    *this = m;
}

template <typename T>
Matrix4_<T> Matrix4_<T>::Tranposed() const
{
    return {
        _00, _10, _20, _30,
        _01, _11, _21, _31,
        _02, _12, _22, _32,
        _03, _13, _23, _33
    };
}

template <typename T>
bool Matrix4_<T>::Inversed(Matrix4_<T>& res) const
{
    float32 d = Determinant();
    if (Math::IsZero(d))
        return false;

    const Matrix4& m = *this;

    d = 1.0f / d;

    res._00 = d * (_11 * (_22 * _33 - _32 * _23) + _21 * (_32 * _13 - _12 * _33) + _31 * (_12 * _23 - _22 * _13));
    res._10 = d * (_12 * (_20 * _33 - _30 * _23) + _22 * (_30 * _13 - _10 * _33) + _32 * (_10 * _23 - _20 * _13));
    res._20 = d * (_13 * (_20 * _31 - _30 * _21) + _23 * (_30 * _11 - _10 * _31) + _33 * (_10 * _21 - _20 * _11));
    res._30 = d * (_10 * (_31 * _22 - _21 * _32) + _20 * (_11 * _32 - _31 * _12) + _30 * (_21 * _12 - _11 * _22));
    res._01 = d * (_21 * (_02 * _33 - _32 * _03) + _31 * (_22 * _03 - _02 * _23) + _01 * (_32 * _23 - _22 * _33));
    res._11 = d * (_22 * (_00 * _33 - _30 * _03) + _32 * (_20 * _03 - _00 * _23) + _02 * (_30 * _23 - _20 * _33));
    res._21 = d * (_23 * (_00 * _31 - _30 * _01) + _33 * (_20 * _01 - _00 * _21) + _03 * (_30 * _21 - _20 * _31));
    res._31 = d * (_20 * (_31 * _02 - _01 * _32) + _30 * (_01 * _22 - _21 * _02) + _00 * (_21 * _32 - _31 * _22));
    res._02 = d * (_31 * (_02 * _13 - _12 * _03) + _01 * (_12 * _33 - _32 * _13) + _11 * (_32 * _03 - _02 * _33));
    res._12 = d * (_32 * (_00 * _13 - _10 * _03) + _02 * (_10 * _33 - _30 * _13) + _12 * (_30 * _03 - _00 * _33));
    res._22 = d * (_33 * (_00 * _11 - _10 * _01) + _03 * (_10 * _31 - _30 * _11) + _13 * (_30 * _01 - _00 * _31));
    res._32 = d * (_30 * (_11 * _02 - _01 * _12) + _00 * (_31 * _12 - _11 * _32) + _10 * (_01 * _32 - _31 * _02));
    res._03 = d * (_01 * (_22 * _13 - _12 * _23) + _11 * (_02 * _23 - _22 * _03) + _21 * (_12 * _03 - _02 * _13));
    res._13 = d * (_02 * (_20 * _13 - _10 * _23) + _12 * (_00 * _23 - _20 * _03) + _22 * (_10 * _03 - _00 * _13));
    res._23 = d * (_03 * (_20 * _11 - _10 * _21) + _13 * (_00 * _21 - _20 * _01) + _23 * (_10 * _01 - _00 * _11));
    res._33 = d * (_00 * (_11 * _22 - _21 * _12) + _10 * (_21 * _02 - _01 * _22) + _20 * (_01 * _12 - _11 * _02));
    return true;
}

template <typename T>
T& Matrix4_<T>::operator()(int32 row, int32 col)
{
    return m[row][col];
}

template <typename T>
const T& Matrix4_<T>::operator()(int32 row, int32 col) const
{
    return m[row][col];
}

template <typename T>
Vector3_<T> Matrix4_<T>::GetTranslation() const
{
    return { _30, _31, _32 };
}

template <typename T>
void Matrix4_<T>::SetTranslation(const Vector3_<T>& t)
{
    _30 = t.x;
    _31 = t.y;
    _32 = t.z;
}

template <typename T>
Vector3_<T> Matrix4_<T>::GetScale() const
{
    return Vector3(
        Vector3_<T>{ _00, _01, _02 }.Length(), 
        Vector3_<T>{ _10, _11, _12 }.Length(), 
        Vector3_<T>{ _20, _21, _22 }.Length());
}

template <typename T>
Matrix4_<T>& Matrix4_<T>::operator *=(const Matrix4_<T>& m)
{
    Matrix4_<T> c = *this;
    _00 = c._00 * m._00 + c._01 * m._10 + c._02 * m._20 + c._03 * m._30;
    _01 = c._00 * m._01 + c._01 * m._11 + c._02 * m._21 + c._03 * m._31;
    _02 = c._00 * m._02 + c._01 * m._12 + c._02 * m._22 + c._03 * m._32;
    _03 = c._00 * m._03 + c._01 * m._13 + c._02 * m._23 + c._03 * m._33;

    _10 = c._10 * m._00 + c._11 * m._10 + c._12 * m._20 + c._13 * m._30;
    _11 = c._10 * m._01 + c._11 * m._11 + c._12 * m._21 + c._13 * m._31;
    _12 = c._10 * m._02 + c._11 * m._12 + c._12 * m._22 + c._13 * m._32;
    _13 = c._10 * m._03 + c._11 * m._13 + c._12 * m._23 + c._13 * m._33;

    _20 = c._20 * m._00 + c._21 * m._10 + c._22 * m._20 + c._23 * m._30;
    _21 = c._20 * m._01 + c._21 * m._11 + c._22 * m._21 + c._23 * m._31;
    _22 = c._20 * m._02 + c._21 * m._12 + c._22 * m._22 + c._23 * m._32;
    _23 = c._20 * m._03 + c._21 * m._13 + c._22 * m._23 + c._23 * m._33;

    _30 = c._30 * m._00 + c._31 * m._10 + c._32 * m._20 + c._33 * m._30;
    _31 = c._30 * m._01 + c._31 * m._11 + c._32 * m._21 + c._33 * m._31;
    _32 = c._30 * m._02 + c._31 * m._12 + c._32 * m._22 + c._33 * m._32;
    _33 = c._30 * m._03 + c._31 * m._13 + c._32 * m._23 + c._33 * m._33;
    return *this;
}

template <typename T>
inline Matrix4_<T> Matrix4_<T>::BuildRotation(const Vector3_<T>& axs, float32 angle)
{
    float32 cosA = std::cos(angle);
    float32 sinA = std::sin(angle);
    Matrix4_<T> m = Identity();
    m._00 = cosA + (1.0f - cosA) * axs.x * axs.x;
    m._01 = (1.0f - cosA) * axs.x * axs.y - axs.z * sinA;
    m._02 = (1.0f - cosA) * axs.x * axs.z + axs.y * sinA;

    m._10 = (1.0f - cosA) * axs.x * axs.y + axs.z * sinA;
    m._11 = cosA + (1.0f - cosA) * axs.y * axs.y;
    m._12 = (1.0f - cosA) * axs.y * axs.z - axs.x * sinA;

    m._20 = (1.0f - cosA) * axs.x * axs.z - axs.y * sinA;
    m._21 = (1.0f - cosA) * axs.y * axs.z + axs.x * sinA;
    m._22 = cosA + (1.0f - cosA) * axs.z * axs.z;
    return m;
}

template <typename T>
inline Matrix4_<T> Matrix4_<T>::BuildRotationX(float32 angle)
{
    float32 cosA = std::cos(angle);
    float32 sinA = std::sin(angle);
    return
    {
        static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f),
        static_cast<T>(0.0f), static_cast<T>(cosA), static_cast<T>(-sinA), static_cast<T>(0.0f),
        static_cast<T>(0.0f), static_cast<T>(sinA), static_cast<T>(cosA), static_cast<T>(0.0f),
        static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(1.0f)
    };
}

template <typename T>
inline Matrix4_<T> Matrix4_<T>::BuildRotationY(float32 angle)
{
    float32 cosA = std::cos(angle);
    float32 sinA = std::sin(angle);
    return
    {
        static_cast<T>(cosA), static_cast<T>(0.0f), static_cast<T>(sinA), static_cast<T>(0.0f),
        static_cast<T>(0.0f), static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f),
        static_cast<T>(-sinA), static_cast<T>(0.0f), static_cast<T>(cosA), static_cast<T>(0.0f),
        static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(1.0f)
    };
}

template <typename T>
inline Matrix4_<T> Matrix4_<T>::BuildRotationZ(float32 angle)
{
    float32 cosA = std::cos(angle);
    float32 sinA = std::sin(angle);
    return
    {
        static_cast<T>(cosA), static_cast<T>(-sinA), static_cast<T>(0.0f), static_cast<T>(0.0f),
        static_cast<T>(sinA), static_cast<T>(cosA), static_cast<T>(0.0f), static_cast<T>(0.0f),
        static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(1.0f), static_cast<T>(0.0f),
        static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(1.0f)
    };
}

template <typename T>
inline Matrix4_<T> Matrix4_<T>::BuildTranslation(const Vector3_<T>& t)
{
    return
    {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        t.x, t.y, t.z, 1.0f
    };
}

template <typename T>
inline Matrix4_<T> Matrix4_<T>::BuildScale(const Vector3_<T>& s)
{
    return
    {
        s.x, 0.0f, 0.0f, 0.0f,
        0.0f, s.y, 0.0f, 0.0f,
        0.0f, 0.0f, s.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

template <typename T>
Matrix4_<T> Matrix4_<T>::BuildLookAt(const Vector3& position, const Vector3& target, const Vector3& upWorld)
{
    Vector3_<T> right, fwd, up;

    fwd = target - position;
    fwd.Normalize();

    right = Vector3_<T>::Cross(upWorld, fwd);
    right.Normalize();

    up = Vector3_<T>::Cross(fwd, right);
    up.Normalize();

    Matrix4_<T> m = Identity();

    m._00 = right.x;
    m._10 = right.y;
    m._20 = right.z;
    m._30 = -Vector3_<T>::Dot(position, right);

    m._01 = up.x;
    m._11 = up.y;
    m._21 = up.z;
    m._31 = -Vector3_<T>::Dot(position, up);

    m._02 = fwd.x;
    m._12 = fwd.y;
    m._22 = fwd.z;
    m._32 = -Vector3_<T>::Dot(position, fwd);

    return m;
}

template <typename T>
inline Matrix4_<T> Matrix4_<T>::BuildProjectionFov(float32 fovY, float32 aspect, float32 zNear, float32 zFar)
{
    Matrix4_<T> m = {};

    float32 sinF2 = std::sin(fovY / 2.0f);
    float32 cosF2 = std::cos(fovY / 2.0f);

    float32 h = cosF2 / sinF2;
    float32 w = h / aspect;

    m._00 = w;
    m._11 = h;
    m._22 = zFar / (zFar - zNear);
    m._32 = (-zNear * zFar) / (zFar - zNear);
    m._23 = 1.0f;
    return m;
}

template <typename T>
inline Matrix4_<T> Matrix4_<T>::BuildOrtho(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
{
    Matrix4_<T> m = {};

    m._00 = 2.0f / (right - left);
    m._11 = 2.0f / (top - bottom);
    m._22 = 1.0f / (zFar - zNear);
    m._32 = (zNear) / (zNear - zFar);
    m._30 = (left + right) / (left - right);
    m._31 = (bottom + top) / (bottom - top);
    m._33 = 1.0f;
    return m;
}

template <typename T>
inline const Matrix4_<T>& Matrix4_<T>::Identity()
{
    static const Matrix4_<T> identity(
                                    static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f),
                                    static_cast<T>(0.0f), static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f),
                                    static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(1.0f), static_cast<T>(0.0f),
                                    static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(1.0f)
                                );
    return identity;
}

using Matrix4 = Matrix4_<float32>;
}
