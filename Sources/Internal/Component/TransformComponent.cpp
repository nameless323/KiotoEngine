//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Component/TransformComponent.h"

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
}
}