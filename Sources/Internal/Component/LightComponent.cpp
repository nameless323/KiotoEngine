#include "stdafx.h"

#include "Component/LightComponent.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{

Component* LightComponent::Clone() const
{
    LightComponent* newComponent = new LightComponent();
    newComponent->m_light = m_light;
    return newComponent;
}

void LightComponent::Serialize(YAML::Emitter& out) const
{
    using ::operator<<;
    out << YAML::Key << "LightType" << YAML::Value << uint16(m_light.LightType);
    out << YAML::Key << "LightDir" << YAML::Value << m_light.Direction;
    out << YAML::Key << "LightColor" << YAML::Value << m_light.Color;
    out << YAML::Key << "LightAtten" << YAML::Value << m_light.AttenuationDistance;
}

void LightComponent::Deserialize(const YAML::Node& in)
{
    if (in["LightType"])
        m_light.LightType = Renderer::eLightType(in["LightType"].as<uint16>());
    if (in["LightDir"])
        m_light.Direction = in["LightDir"].as<Vector3>();
    if (in["LightColor"])
        m_light.Color = in["LightColor"].as<Renderer::Color>();
    if (in["LightAtten"])
        m_light.AttenuationDistance = in["LightAtten"].as<float32>();
}

}