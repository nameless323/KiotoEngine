#pragma once

#include "Math/Vector3.h"
#include "Render/Color.h"
#include "Render/Shaders/autogen/CommonStructures.h"

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

    Matrix4 ShadowMatrix{};
    bool CastShadow = false;

    Light& operator=(const Light& other) = default;

    SInp::Light GetGraphicsLight() const;
};

inline SInp::Light Light::GetGraphicsLight() const
{
    static_assert(sizeof(SInp::Light) == 64); // [a_vorontcov] not 100% fool proof, but will work for now. Assert means that changes in hlsl light aren't properly reflected here. Update this method and update the assert.
    SInp::Light light;
    light.Position = Position;
    light.Type = static_cast<uint32>(LightType);
    light.Color = Color.ToVector4();
    light.Direction = Direction;
    light.Pad0 = Pad0;
    light.Data = Data;
    return light;
}
};
