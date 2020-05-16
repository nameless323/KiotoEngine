#pragma once

#if _WIN32 || _WIN64
#include <DirectXMath.h>
#endif

#include "Core/CoreTypes.h"

#include <iostream>
#include <iomanip>

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
    Vector3_(const Vector3_<T>& v);

    bool operator== (const Vector3_<T>& other) const;
    bool operator!= (const Vector3_<T>& other) const;

    Vector3_<T>& operator=(const Vector3_<T>& v);

    Vector3_<T>& operator+=(const Vector3_<T>& v);
    Vector3_<T>& operator-=(const Vector3_<T>& v);
    Vector3_<T>& operator*=(float32 t);
    Vector3_<T> operator-() const;

#if _WIN32 || _WIN64
    explicit Vector3_(const DirectX::XMFLOAT3& vec);
    operator DirectX::XMFLOAT3() const;
#endif
    ///
    /// Get square length of the vector.
    ///
    float32 SqrLength() const;
    ///
    /// Get length of the vector.
    ///
    float32 Length() const;
    ///
    /// Normalize vector.
    ///
    Vector3_<T>& Normalize();
    ///
    /// Get normalized copy of vector.
    ///
    Vector3_<T> Normalized() const;

    ///
    /// Dot product of the v1 and v2 vectors.
    ///
    static T Dot(const Vector3_<T>& v1, const Vector3_<T>& v2);
    ///
    /// Cross product of the v1 and v2 vectors.
    ///
    static Vector3_<T> Cross(const Vector3_<T>& v1, const Vector3_<T>& v2);
    ///
    /// Normalize vector.
    ///
    static Vector3_<T> Normalized(Vector3_<T> v);

    static const Vector3_<T> Up;
    static const Vector3_<T> Down;
    static const Vector3_<T> Left;
    static const Vector3_<T> Right;
    static const Vector3_<T> Forward;
    static const Vector3_<T> Backward;
    static const Vector3_<T> Zero;

    friend std::ostream& operator<<(std::ostream& os, const Vector3_<float32>& M);
};

template<typename T>
Vector3_<T> operator+ (Vector3_<T> v1, const Vector3_<T>& v2)
{
    v1 += v2;
    return v1;
}

template<typename T>
Vector3_<T> operator- (Vector3_<T> v1, const Vector3_<T>& v2)
{
    v1 -= v2;
    return v1;
}

template<typename T>
Vector3_<T> operator* (Vector3_<T> v, float32 t)
{
    v *= t;
    return v;
}

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
Vector3_<T>::Vector3_(const Vector3_<T>& v)
    : x(v.x), y(v.y), z(v.z)
{}

template <typename T>
inline bool Vector3_<T>::operator==(const Vector3_<T>& other) const
{
    return Math::IsFloatEqual(x, other.x) && Math::IsFloatEqual(y, other.y)
        && Math::IsFloatEqual(z, other.z);
}

template <>
inline bool Vector3_<int32>::operator==(const Vector3_<int32>& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

template <>
inline bool Vector3_<int64>::operator==(const Vector3_<int64>& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

template <typename T>
bool Vector3_<T>::operator!=(const Vector3_<T>& other) const
{
    return !(*this == other);
}

template <typename T>
Vector3_<T>& Vector3_<T>::operator=(const Vector3_<T>& v)
{
    x = v.x;
    y = v.y;
    z = v.z;

    return *this;
}

template <typename T>
Vector3_<T>& Vector3_<T>::operator+=(const Vector3_<T>& v)
{
    x += v.x;
    y += v.y;
    z += v.z;

    return *this;
}

template <typename T>
Vector3_<T>& Vector3_<T>::operator-=(const Vector3_<T>& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;

    return *this;
}

template <typename T>
Vector3_<T>& Vector3_<T>::operator*=(float32 t)
{
    x *= t;
    y *= t;
    z *= t;

    return *this;
}

template <typename T>
Vector3_<T> Vector3_<T>::operator-() const
{
    return { -x, -y, -z};
}

#if _WIN32 || _WIN64
template <typename T>
Vector3_<T>::Vector3_(const DirectX::XMFLOAT3& v)
    : x(v.x), y(v.y), z(v.z)
{
}

template <typename T>
Vector3_<T>::operator DirectX::XMFLOAT3() const
{
    return XMFLOAT3(x, y, z);
}
#endif

template <typename T>
float32 Vector3_<T>::SqrLength() const
{
    return x * x + y * y + z * z;
}

template <typename T>
inline float32 Vector3_<T>::Length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

template <typename T>
Vector3_<T>& Vector3_<T>::Normalize()
{
    float32 invLen = 1.f / Length(); // [a_vorontcov] Assert sqrLen > Epsilon * Epsilon maybe?
    x *= invLen;
    y *= invLen;
    z *= invLen;
    return *this;
}

template <typename T>
Vector3_<T> Vector3_<T>::Normalized() const
{
    return Vector3_<T>::Normalized(*this);
}

template <typename T>
Vector3_<T> Vector3_<T>::Normalized(Vector3_<T> v)
{
    return v.Normalize();
}

template <typename T>
T Vector3_<T>::Dot(const Vector3_<T>& v1, const Vector3_<T>& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T>
Vector3_<T> Vector3_<T>::Cross(const Vector3_<T>& v1, const Vector3_<T>& v2)
{
    return { v1.y * v2.z - v2.y * v1.z, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x };
}

template <typename T>
const Vector3_<T> Vector3_<T>::Up((T)0, (T)1, (T)0);

template <typename T>
const Vector3_<T> Vector3_<T>::Down((T)0, (T)-1, (T)0);

template <typename T>
const Vector3_<T> Vector3_<T>::Left((T)-1, (T)0, (T)0);

template <typename T>
const Vector3_<T> Vector3_<T>::Right((T)1, (T)0, (T)0);

template <typename T>
const Vector3_<T> Vector3_<T>::Forward((T)0, (T)0, (T)1);

template <typename T>
const Vector3_<T> Vector3_<T>::Backward((T)0, (T)0, (T)-1);

template <typename T>
const Vector3_<T> Vector3_<T>::Zero((T)0, (T)0, (T)0);

using Vector3 = Vector3_<float32>;
using Vector3i = Vector3_<int32>;

inline std::ostream& operator<<(std::ostream& os, const Vector3_<float32>& V)
{
    std::ios_base::fmtflags f(os.flags());
    os << std::setprecision(5) << std::fixed << std::internal;
    for (uint32 i = 0; i < 3; ++i)
        os << " | " << std::setw(6) << V.data[i];
    os.flags(f);
    return os;
}
}