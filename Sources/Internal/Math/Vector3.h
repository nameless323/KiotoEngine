//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#if _WIN32 || _WIN64
#include <DirectXMath.h>
#endif

#include "Core/CoreTypes.h"

namespace Kioto
{
template <typename T>
class Vector3_
{
public:
    union
    {
        struct
        {
            T x; T y; T z;
        };
        T data[3];
    };

    Vector3_();
    explicit Vector3_(T t);
    Vector3_(T x_, T y_, T z_);
    Vector3_(const Vector3_<T>& other);

#if _WIN32 || _WIN64
    explicit Vector3_(const DirectX::XMFLOAT3& vec);
    operator DirectX::XMFLOAT3() const;
#endif
    float32 SqrLength() const;
    float32 Length() const;
    Vector3_<T>& Normalize();

    static T Dot(const Vector3_<T>& v1, const Vector3_<T>& v2);
    static Vector3_<T> Normalized(Vector3_<T> v);
};

template <typename T>
Vector3_<T>::Vector3_()
{
    memset(data, 0, sizeof(data));
}

template <typename T>
Vector3_<T>::Vector3_(T t)
    : x(t), y(t), z(t)
{
}

template <typename T>
Vector3_<T>::Vector3_(T x_, T y_, T z_)
    : x(x_), y(y_), z(z_)
{
}

template <typename T>
Vector3_<T>::Vector3_(const Vector3_<T>& other)
    : x(other.x), y(other.y), z(other.z)
{}

#if _WIN32 || _WIN64
template <typename T>
Vector3_<T>::Vector3_(const DirectX::XMFLOAT3& vec)
    : x(vec.x), y(vec.y), z(vec.z)
{
}

template <typename T>
Vector3_<T>::operator DirectX::XMFLOAT3() const
{
    return XMFLOAT3(x, y, z);
}
#endif

template <typename T>
inline float32 Vector3_<T>::SqrLength() const
{
    return x * x + y * y + z * z;
}

template <typename T>
inline float32 Vector3_<T>::Length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

template <typename T>
inline Vector3_<T>& Vector3_<T>::Normalize()
{
    float32 invLen = 1.f / Length(); // [a_vorontsov] Assert sqrLen > Epsilon * Epsilon maybe?
    x *= invLen;
    y *= invLen;
    z *= invLen;
    return *this;
}

template <typename T>
inline Vector3_<T> Vector3_<T>::Normalized(Vector3_<T> v)
{
    return v.Normalize();
}

template <typename T>
inline T Vector3_<T>::Dot(const Vector3_<T>& v1, const Vector3_<T>& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

using Vector3 = Vector3_<float32>;
using Vector3i = Vector3_<int32>;
}