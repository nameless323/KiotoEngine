//
// Copyright (C) Aleksandr Vorontcov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Math/MathHelpers.h"
#include "Math/Vector3.h"

namespace Kioto
{
template <typename T>
class Matrix3_
{
public:
    union
    {
        Vector3_<T> r[3];
        struct
        {
            T _00, _01, _02;
            T _10, _11, _12;
            T _20, _21, _22;
        };
        T m[3][3];
        T data[9];
    };

    Matrix3_();
    Matrix3_(const Matrix3_<T>& other);
    Matrix3_(const Vector3_<T>& r0, const Vector3_<T>& r1, const Vector3_<T>& r2);
    Matrix3_(
        T m_00, T m_01, T m_02,
        T m_10, T m_11, T m_12,
        T m_20, T m_21, T m_22
    );

    Matrix3_<T>& operator= (const Matrix3_<T>& other);

    static const Matrix3_<T> Identity;
};


template <typename T>
Matrix3_<T>::Matrix3_()
{
    memset(data, 0, sizeof(data));
}

template <typename T>
Matrix3_<T>::Matrix3_(const Matrix3_<T>& other)
    : _00(other._00), _01(other._01), _02(other._02),
      _10(other._10), _11(other._11), _12(other._12),
      _20(other._20), _21(other._21), _22(other._22)
{
}

template <typename T>
Matrix3_<T>::Matrix3_(const Vector3_<T>& r0, const Vector3_<T>& r1, const Vector3_<T>& r2)
{
    r[0] = r0; r[1] = r1; r[2] = r2; r[3] = r3;
}

template <typename T>
Matrix3_<T>::Matrix3_(
    T m_00, T m_01, T m_02,
    T m_10, T m_11, T m_12,
    T m_20, T m_21, T m_22
)
    : _00(m_00), _01(m_01), _02(m_02),
      _10(m_10), _11(m_11), _12(m_12),
      _20(m_20), _21(m_21), _22(m_22)
{
}

template <typename T>
Matrix3_<T>& Matrix3_<T>::operator=(const Matrix3_<T>& other)
{
    _00 = other._00; _01 = other._01; _02 = other._02; // [a_vorontcov] TODO: memcpy maybe?
    _10 = other._10; _11 = other._11; _12 = other._12;
    _20 = other._20; _21 = other._21; _22 = other._22;
    return *this;
}

template <typename T>
const Matrix3_<T> Matrix3_<T>::Identity(
    static_cast<T>(1.0f), static_cast<T>(0.0f), static_cast<T>(0.0f),
    static_cast<T>(0.0f), static_cast<T>(1.0f), static_cast<T>(0.0f),
    static_cast<T>(0.0f), static_cast<T>(0.0f), static_cast<T>(1.0f)
);

using Matrix3 = Matrix3_<float32>;
}