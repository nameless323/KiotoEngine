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
    out << YAML::Key << "Component";
    out << YAML::Value << GetTypeS();
    out << YAML::Comment(GetTypeName());
}
}