#pragma once

#include "Core/CoreTypes.h"
#include "Core/CoreHelpers.h"
#include "Core/ECS/Component.h"

#include "Render/Lighting/Light.h"

namespace Kioto
{
class LightComponent : public Component
{
    DECLARE_COMPONENT(LightComponent);

public:
    KIOTO_API LightComponent() = default;
    KIOTO_API ~LightComponent() = default;

    Component* Clone() const override;

    void Serialize(YAML::Emitter& out) const override;
    void Deserialize(const YAML::Node& in) override;

private:
    Renderer::Light m_light{};
};
REGISTER_COMPONENT(LightComponent);
}