#pragma once

#if _WIN32 || _WIN64
#include <DirectXMath.h>
#endif

#include "Core/CoreTypes.h"
#include "Math/Vector3.h"

#include <iostream>
#include <iomanip>

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

    Vector4_();
    explicit Vector4_(T t);
    explicit Vector4_(const Vector3_<T>& vec, T w_ = static_cast<T>(0));
    Vector4_(T x_, T y_, T z_, T w_);
    Vector4_(const Vector4_& other);

    Vector4_<T> operator-() const;

    bool operator== (const Vector4_<T>& other) const;
    bool operator!= (const Vector4_<T>& other) const;

    Vector3_<T> GetVec3() const;

#if _WIN32 || _WIN64
    explicit Vector4_(const DirectX::XMFLOAT4& vec);
    operator DirectX::XMFLOAT4() const;
#endif

    static const Vector4_<T> Up;
    static const Vector4_<T> Down;
    static const Vector4_<T> Left;
    static const Vector4_<T> Right;
    static const Vector4_<T> Forward;
    static const Vector4_<T> Backward;

    friend std::ostream& operator<<(std::ostream& os, const Vector4_<float32>& M);
};

template <typename T>
Vector4_<T>::Vector4_()
{
    memset(data, 0, sizeof(data));
}

template <typename T>
Vector4_<T>::Vector4_(T t)
    : x(t), y(t), z(t), w(t)
{
}

template <typename T>
Vector4_<T>::Vector4_(const Vector3_<T>& vec, T w_)
    : x(vec.x), y(vec.y), z(vec.z), w(w_)
{
}

template <typename T>
Vector4_<T>::Vector4_(T x_, T y_, T z_, T w_)
    : x(x_), y(y_), z(z_), w(w_)
{
}

template <typename T>
Vector4_<T>::Vector4_(const Vector4_& other)
    : x(other.x), y(other.y), z(other.z), w(other.w)
{
}

template <typename T>
Vector4_<T> Vector4_<T>::operator-() const
{
    return { -x, -y, -z, -w };
}

template <typename T>
bool Vector4_<T>::operator==(const Vector4_<T>& other) const
{
    return Math::IsFloatEqual(x, other.x) && Math::IsFloatEqual(y, other.y)
        && Math::IsFloatEqual(z, other.z) && Math::IsFloatEqual(w, other.w);
}

template <>
bool Vector4_<int32>::operator ==(const Vector4_<int32>& other) const
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

template <>
bool Vector4_<int64>::operator ==(const Vector4_<int64>& other) const
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

template <typename T>
bool Vector4_<T>::operator!=(const Vector4_<T>& other) const
{
    return !(*this == other);
}

template <typename T>
Vector3_<T> Vector4_<T>::GetVec3() const
{
    return { x, y, z };
}

#if _WIN32 || _WIN64
template <typename T>
Vector4_<T>::Vector4_(const DirectX::XMFLOAT4& vec)
    : x(vec.x), y(vec.y), z(vec.z), w(vec.w)
{
}

template <typename T>
Vector4_<T>::operator DirectX::XMFLOAT4() const
{
    return XMFLOAT4(x, y, z, w);
}
#endif

template <typename T>
const Vector4_<T> Vector4_<T>::Up(0.0f, 1.0f, 0.0f, 0.0f);

template <typename T>
const Vector4_<T> Vector4_<T>::Down(0.0f, -1.0f, 0.0f, 0.0f);

template <typename T>
const Vector4_<T> Vector4_<T>::Left(-1.0f, 0.0f, 0.0f, 0.0f);

template <typename T>
const Vector4_<T> Vector4_<T>::Right(1.0f, 0.0f, 0.0f, 0.0f);

template <typename T>
const Vector4_<T> Vector4_<T>::Forward(0.0f, 0.0f, 1.0f, 0.0f);

template <typename T>
const Vector4_<T> Vector4_<T>::Backward(0.0f, 0.0f, -1.0f, 0.0f);

using Vector4 = Vector4_<float32>;
using Vector4i = Vector4_<int32>;

std::ostream& operator<<(std::ostream& os, const Vector4_<float32>& V)
{
    std::ios_base::fmtflags f(os.flags());
    os << std::setprecision(5) << std::fixed << std::internal;
    for (uint32 i = 0; i < 4; ++i)
        os << " | " << std::setw(6) << V.data[i];
    os.flags(f);
    return os;
}
}