//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <cmath>

#include "Core/CoreTypes.h"

namespace Kioto::Math
{
#define FLOAT_TEMPLATE template <typename T, typename = std::enable_if_t<std::is_same_v<T, double> || std::is_same_v<T, float>>>

constexpr float64 PI_Double = 3.141592653589793238;
constexpr float64 TwoPI_Double = PI_Double * 2.0;
constexpr float64 PIOverTwo_Double = PI_Double / 2.0;
constexpr float64 PIOverFour_Double = PI_Double / 4.0;
constexpr float64 PIOverSix_Double = PI_Double / 6.0;

constexpr float32 PI = 3.1415927f;
constexpr float32 TwoPI = PI * 2.0f;
constexpr float32 PIOverTwo = PI / 2.0f;
constexpr float32 PIOverFour = PI / 4.0f;
constexpr float32 PIOverSix = PI / 6.0f;

template <typename T>
struct DecomposedValue
{
    T FractPart = {};
    int32 IntPart = 0;
};

///
/// Decomposes the value into integer and fractional parts.
///
FLOAT_TEMPLATE
inline DecomposedValue<T> Decompose(T val)
{
    DecomposedValue<T> d;
    T intP = 0.0f;
    d.FractPart = modf(val, &intP);
    d.IntPart = static_cast<int32>(intP);
    return d;
}

///
/// Returns largest integer not greater than a val.
///
FLOAT_TEMPLATE
inline T Floor(T val)
{
    DecomposedValue<T> dVal;
    dVal = Decompose(val);
    return val - dVal.FractPart;
}

///
/// Returns smallest integer not less than a val.
///
FLOAT_TEMPLATE
inline T Ceil(T val)
{
    return Floor(val) + static_cast<T>(1);
}

///
/// Clamps val between a and b.
///
FLOAT_TEMPLATE
inline T Clamp(T val, T low, T top)
{
    return max(a, min(b, x));
}

///
/// Clamps val between 0 and 1.
///
FLOAT_TEMPLATE
inline T Saturate(T val)
{
    return Clamp(val, static_cast<T>(0.0f), static_cast<T>(1.0f));
}

///
/// Converts angle in radians to angle in degrees.
///
template <typename T>
inline constexpr auto RadToDeg(T rad) -> decltype(rad * (180.0f / PI))
{
    return rad * (180.0f / PI);
}

///
/// Converts angle in degrees to angle in radians.
///
template <typename T>
inline constexpr auto DegToRad(T deg) -> decltype(deg * PI / 180.f)
{
    return deg * PI / 180.f;
}

#undef FLOAT_TEMPLATE
}