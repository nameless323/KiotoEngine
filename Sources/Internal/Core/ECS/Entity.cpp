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
    m_components = std::move(other.m_components);
    other.m_components.clear();
}

Entity& Entity::operator=(const Entity& other) // [a_vorontsov] TODO: copy swap.
{
    if (this == &other)
        return *this;
    for (auto component : m_components)
        SafeDelete(component);
    m_components.clear();
    for (auto component : other.m_components)
        AddComponent(component->Clone());
    return *this;
}

Entity& Entity::operator=(Entity&& other)
{
    for (auto component : m_components)
        SafeDelete(component);
    m_components.clear();
    m_components = std::move(other.m_components);
    other.m_components.clear();
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