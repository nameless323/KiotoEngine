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
class Vector2_
{
public:
    union
    {
        struct
        {
            T x; T y;
        };
        struct
        {
            T u; T v;
        };
        T data[2];
    };

    Vector2_();
    explicit Vector2_(T t);
    Vector2_(T x_, T y_);
    Vector2_(const Vector2_& other);

    // TODO:: operator== via memcmp data.
};

template <typename T>
Vector2_<T>::Vector2_()
    : x(0), y(0)
{
}

template <typename T>
Vector2_<T>::Vector2_(T t)
    : x(t), y(t)
{
}

template <typename T>
Vector2_<T>::Vector2_(T x_, T y_)
    : x(x_), y(y_)
{
}

template <typename T>
Vector2_<T>::Vector2_(const Vector2_& other)
    : x(other.x), y(other.y)
{
}

using Vector2 = Vector2_<float32>;
using Vector2i = Vector2_<int32>;
}