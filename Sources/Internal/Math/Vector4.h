//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#if _WIN32 || _WIN64
#include <DirectXMath.h>
#endif

namespace Kioto
{
template <typename T>
class Vector4_
{
public:
    union
    {
        struct
        {
            T x; T y; T z; T w;
        };
        T data[4];
    };

    Vector4_()
    {
        memset(data, 0, sizeof(data)); // TODO: check if sizeof data is ok.
    }

    explicit Vector4_(T t)
        : x(t), y(t), z(t), w(t)
    {
    }

    Vector4_(T x_, T y_, T z_, T w_)
        : x(x_), y(y_), z(z_), w(w_)
    {
    }

    Vector4_(const Vector4_& other)
        : x(other.x), y(other.y), z(other.z), w(other.w)
    {}

#if _WIN32 || _WIN64
    explicit Vector4_(const DirectX::XMFLOAT4& vec)
        : x(vec.x), y(vec.y), z(vec.z), w(vec.w)
    {
    }
    
    operator DirectX::XMFLOAT4() const
    {
        return XMFLOAT4(x, y, z, w);
    }
#endif
};

using Vector4 = Vector4_<float32>;
using Vector4i = Vector4_<int32>;
}