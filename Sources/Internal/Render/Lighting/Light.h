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
    Vector4 Data{}; // [a_vorontcov] See Lighting.kincl Light struct for more details.

    Light& operator=(const Light& other) = default;
};
};
