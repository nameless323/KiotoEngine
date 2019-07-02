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
    BEGIN_COMPONENT_SERIALIZARION();

    out << YAML::Key << "MatPath" << YAML::Value << m_materialPath;
    out << YAML::Key << "MeshPath" << YAML::Value << m_meshPath;

    END_COMPONENT_SERIALIZATION();
}
}