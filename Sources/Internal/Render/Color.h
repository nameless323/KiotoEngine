#pragma once

#include "Math/Vector4.h"

namespace Kioto::Renderer
{
class Color
{
public:
    union
    {
        struct
        {
            float32 r; float32 g; float32 b; float32 a;
        };
        float32 data[4];
    };

    Color();
    Color(float32 r_, float32 g_, float32 b_, float32 a_);
    Vector4 ToVector4() const;

    static const Color DefaultBackgroundColor;
    static const Color Black;
    static const Color White;
};

inline Color::Color() : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
{
}

inline Color::Color(float32 r_, float32 g_, float32 b_, float32 a_)
    : r(r_), g(g_), b(b_), a(a_)
{
}

inline Vector4 Color::ToVector4() const
{
    return { r, g, b, a };
}

inline const Color Color::DefaultBackgroundColor(0.45f, 0.45f, 0.45f, 1.0f);
inline const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
inline const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
}