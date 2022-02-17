#include "Component/LightComponent.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{

Component* LightComponent::Clone() const
{
    LightComponent* newComponent = new LightComponent();
    newComponent->mLight = mLight;
    return newComponent;
}

void LightComponent::Serialize(YAML::Emitter& out) const
{
    using ::operator<<;
    out << YAML::Key << "LightType" << YAML::Value << uint16(mLight.LightType);
    out << YAML::Key << "LightDir" << YAML::Value << mLight.Direction;
    out << YAML::Key << "LightColor" << YAML::Value << mLight.Color;
    out << YAML::Key << "LightData" << YAML::Value << mLight.Data;
    out << YAML::Key << "CastShadow" << YAML::Value << mLight.CastShadow;
}

void LightComponent::Deserialize(const YAML::Node& in)
{
    if (in["LightType"])
        mLight.LightType = Renderer::eLightType(in["LightType"].as<uint16>());
    if (in["LightDir"])
        mLight.Direction = in["LightDir"].as<Vector3>();
    if (in["LightColor"])
        mLight.Color = in["LightColor"].as<Renderer::Color>();
    if (in["LightData"])
        mLight.Data = in["LightData"].as<Vector4>();
    if (in["CastShadow"])
        mLight.CastShadow = in["CastShadow"].as<bool>();
}

}