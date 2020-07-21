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
    Color Color{};
    eLightType LightType = eLightType::Directional;
    Vector3 Direction{};
    float AttenuationDistance = 0.0f;
};
};
