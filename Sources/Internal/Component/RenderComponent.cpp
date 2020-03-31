//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Component/RenderComponent.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{
Component* RenderComponent::Clone() const
{
    return new RenderComponent();
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