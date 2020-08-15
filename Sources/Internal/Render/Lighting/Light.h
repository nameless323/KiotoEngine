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
    Color Color{};
    eLightType LightType = eLightType::Directional;
    Vector3 Direction{};
    Vector3 Attenuation{};

    Light& operator=(const Light& other) = default;
};
};
