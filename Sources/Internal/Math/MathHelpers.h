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
    T FractPart = 0.0f;
    int32 IntPart = 0;
};

///
/// Decomposes the value into integer and fractional parts.
///
template <typename T>
inline DecomposedValue<T> Decompose(T val)
{
    static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value, "You try to decompose not the fractional var.");
    DecomposedValue<T> d;
    T intP = 0.0f;
    d.FractPart = modf(val, &intP);
    d.IntPart = static_cast<int32>(intP);
    return d;
}

}