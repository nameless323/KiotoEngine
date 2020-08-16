#pragma once

#include "Math/Vector3.h"
#include "Render/Color.h"

namespace Kioto::Renderer
{
enum class eLightType
{
    Directional = 0,
    Point,
    Spot
};

struct Light
{
    Vector3 Position{};
    eLightType LightType = eLightType::Directional;
    Color Color{};
    Vector3 Direction{};
    float32 Pad0;
    Vector3 Attenuation{};

    Light& operator=(const Light& other) = default;
};
};
