#include "stdafx.h"

#include "Component/RenderComponent.h"

#include "Core/Yaml/YamlParser.h"
#include "Render/RenderObject.h"

namespace Kioto
{
Component* RenderComponent::Clone() const
{
    return new RenderComponent();
}

bool RenderComponent::GetCastShadows() const
{
    return m_renderObject->GetCastShadow();
}

void RenderComponent::SetCastShadows(bool castShadows)
{
    m_renderObject->SetCastShadow(castShadows);
}

bool RenderComponent::GetIsVisible() const
{
    return m_renderObject->GetIsVisible();
}

void RenderComponent::SetIsVisible(bool isVisible)
{
    m_renderObject->SetIsVisible(isVisible);
}

void RenderComponent::Serialize(YAML::Emitter& out) const
{
    out << YAML::Key << "MatPath" << YAML::Value << m_materialPath;
    out << YAML::Key << "MeshPath" << YAML::Value << m_meshPath;
}

void RenderComponent::Deserialize(const YAML::Node& in)
{
    if (in["MatPath"])
        m_materialPath = in["MatPath"].as<std::string>();
    if (in["MeshPath"])
        m_meshPath = in["MeshPath"].as<std::string>();
}
}