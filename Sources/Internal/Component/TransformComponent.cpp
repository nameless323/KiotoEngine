//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Component/TransformComponent.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{
Component* TransformComponent::Clone() const
{
    TransformComponent* t = new TransformComponent();
    t->m_toWorld = m_toWorld;
    t->m_toParent = m_toParent;
    t->m_toModel = m_toModel;

    t->m_children = m_children;
    t->m_parent = m_parent;
    t->m_worldPosition = m_worldPosition;
    t->m_worldRotation = m_worldRotation;

    return t;
}

void TransformComponent::Save(YAML::Emitter& out) const
{
    BEGIN_COMPONENT_SERIALIZARION();

    out << YAML::Key << "ToWorld" << YAML::Value << m_toWorld;
    out << YAML::Key << "ToParent" << YAML::Value << m_toParent;
    out << YAML::Key << "ToModel" << YAML::Value << m_toModel;
    out << YAML::Key << "WorldPosition" << YAML::Value << m_worldPosition;
    out << YAML::Key << "WorldRotation" << YAML::Value << m_worldRotation;

    END_COMPONENT_SERIALIZATION();
}
}