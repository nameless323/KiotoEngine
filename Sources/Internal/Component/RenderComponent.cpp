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

void RenderComponent::Save(YAML::Emitter& out) const
{
    out << YAML::Key << "Component";
    out << YAML::Value << GetTypeS();
    out << YAML::Comment(GetTypeName());

    out << YAML::Key << "Data";
    out << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "MatPath";
    out << YAML::Value << m_materialPath;
    out << YAML::Key << "MeshPath";
    out << YAML::Value << m_meshPath;

    out << YAML::EndMap;
}
}