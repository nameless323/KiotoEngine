//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <cmath>

#include "Core/CoreTypes.h"

namespace Kioto::Math
{
constexpr float64 PI_Double = 3.141592653589793238;
constexpr float32 PI = 3.1415927f;

template <typename T>
struct DecomposedValue
{
    T FractPart = {};
    int32 IntPart = 0;
};

///
/// Decomposes the value into integer and fractional parts.
///
template <typename T, typename = std::enable_if_t<std::is_same_v<T, double>|| std::is_same_v<T, float>>>
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
template <typename T, typename = std::enable_if_t<std::is_same_v<T, double> || std::is_same_v<T, float>>>
inline T Floor(T val)
{
    DecomposedValue<T> dVal;
    dVal = Decompose(val);
    return val - dVal.FractPart;
}

///
/// Returns smallest integer not less than a val;
///
template <typename T, typename = std::enable_if_t<std::is_same_v<T, double> || std::is_same_v<T, float>>>
inline T Ceil(T val)
{
    return Floor(val) + static_cast<T>(1);
}
}