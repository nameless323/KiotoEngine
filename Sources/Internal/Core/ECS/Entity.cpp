//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/ECS/Entity.h"

#include "Core/CoreHelpers.h"

namespace Kioto
{

Entity::Entity(const Entity& other)
{
    for (auto component : other.m_components)
        AddComponent(component->Clone());
}

Entity::Entity(Entity&& other)
{
    using std::swap;
    swap(*this, other);
}

Entity& Entity::operator=(Entity other)
{
    using std::swap;
    swap(*this, other);
    return *this;
}

Entity::~Entity()
{
    for (auto& component : m_components)
        SafeDelete(component);
    m_components.clear();
}

void Entity::RemoveComponent(Component* component)
{
    auto it = std::find(m_components.begin(), m_components.end(), component);
    if (it != m_components.end())
    {
        delete &(*it);
        m_components.erase(it);
    }
}

void Entity::AddComponent(Component* component)
{
    if (component->GetEntity() != nullptr)
        return; // [a_vorontsov] Do smth scary here.
    component->SetEntity(this);
    m_components.push_back(component);
}

}