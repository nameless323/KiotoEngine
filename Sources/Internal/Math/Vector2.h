#pragma once

#if _WIN32 || _WIN64
#include <DirectXMath.h>
#endif

#include "Core/CoreTypes.h"
#include "Math/MathHelpers.h"

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

    bool operator== (const Vector2_<T>& other) const;
    bool operator!= (const Vector2_<T>& other) const;

    static const Vector2_<T> Zero;
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

template <typename T>
inline bool Vector2_<T>::operator==(const Vector2_<T>& other) const
{
    return Math::IsFloatEqual(x, other.x) && Math::IsFloatEqual(y, other.y);
}

template <>
inline bool Vector2_<int32>::operator==(const Vector2_<int32>& other) const
{
    return x == other.x && y == other.y;
}

template <>
inline bool Vector2_<int64>::operator==(const Vector2_<int64>& other) const
{
    return x == other.x && y == other.y;
}

template <typename T>
bool Vector2_<T>::operator!=(const Vector2_<T>& other) const
{
    return !(*this == other);
}

template <typename T>
const Vector2_<T> Vector2_<T>::Zero((T)0, (T)0);

using Vector2 = Vector2_<float32>;
using Vector2i = Vector2_<int32>;
}