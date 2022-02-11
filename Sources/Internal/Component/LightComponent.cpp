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
    out << YAML::Key << "LightData" << YAML::Value << m_light.Data;
    out << YAML::Key << "CastShadow" << YAML::Value << m_light.CastShadow;
}

void LightComponent::Deserialize(const YAML::Node& in)
{
    if (in["LightType"])
        m_light.LightType = Renderer::eLightType(in["LightType"].as<uint16>());
    if (in["LightDir"])
        m_light.Direction = in["LightDir"].as<Vector3>();
    if (in["LightColor"])
        m_light.Color = in["LightColor"].as<Renderer::Color>();
    if (in["LightData"])
        m_light.Data = in["LightData"].as<Vector4>();
    if (in["CastShadow"])
        m_light.CastShadow = in["CastShadow"].as<bool>();
}

}