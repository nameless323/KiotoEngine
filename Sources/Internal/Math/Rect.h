//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Math/Vector4.h"

namespace Kioto
{
template <typename T>
struct Rect_
{
    Rect_() = default;
    Rect_(const Vector4_<T>& other);
    Rect_(const Rect_& other);
    Rect_(T left, T top, T right, T bottom);
    Rect_& operator= (const Rect_& other);

    T Left = 0;
    T Top = 0;
    T Right = 0;
    T Bottom = 0;
};

template <typename T>
Rect_<T>::Rect_(const Vector4_<T>& other)
    : Left(other.x), Top(other.y), Right(other.z), Bottom(other.w)
{
}

template <typename T>
Rect_<T>::Rect_(const Rect_<T>& other)
    : Left(other.Left), Top(other.Top), Right(other.Right), Bottom(other.Bottom)
{
}

template <typename T>
Rect_<T>::Rect_(T left, T top, T right, T bottom)
    : Left(left), Top(top), Right(right), Bottom(bottom)
{
}

template <typename T>
Rect_<T>& Rect_<T>::operator= (const Rect_& other)
{
    Left = other.Left;
    Top = other.Top;
    Right = other.Right;
    Bottom = other.Bottom;

    return *this;
}

using Rect = Rect_<float32>;
using RectI = Rect_<int32>;
}
