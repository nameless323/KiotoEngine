//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#if _WIN32 || _WIN64
#include <DirectXMath.h>
#endif

#include "Math/Vector4.h"

namespace Kioto
{
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

    Matrix4_<T>& operator= (const Matrix4_<T> other);

    float32 Determinant() const;

    void Transpose();
    Matrix4_<T> Tranposed() const;

    float32& operator()(int32 row, int32 col);
    const float32& operator()(int32 row, int32 col) const;

    static const Matrix4_<T>& Identity();
};

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
Matrix4_<T>& Matrix4_<T>::operator=(const Matrix4_<T> other)
{
    _00 = other._00; _01 = other._01; _02 = other._02; _03 = other._03;
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
float32& Matrix4_<T>::operator()(int32 row, int32 col)
{
    return m[row][col];
}

template <typename T>
const float32& Matrix4_<T>::operator()(int32 row, int32 col) const
{
    return m[row][col];
}

template <typename T>
inline const Matrix4_<T>& Matrix4_<T>::Identity()
{
    static const Matrix4_<T> identity(
                                    static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0),
                                    static_cast<T>(0), static_cast<T>(1), static_cast<T>(0), static_cast<T>(0),
                                    static_cast<T>(0), static_cast<T>(0), static_cast<T>(1), static_cast<T>(0),
                                    static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(1)
                                );
    return identity;
}

using Matrix4 = Matrix4_<float32>;
}
