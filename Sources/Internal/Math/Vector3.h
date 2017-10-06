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
    Vector3_(const Vector3_& other);

#if _WIN32 || _WIN64
    explicit Vector3_(const DirectX::XMFLOAT3& vec);
    operator DirectX::XMFLOAT3() const;
#endif
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
Vector3_<T>::Vector3_(const Vector3_& other)
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

using Vector3 = Vector3_<float32>;
using Vector3i = Vector3_<int32>;
}