//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <vector>

#include "Core/ECS/Component.h"

namespace Kioto
{
class Entity
{
public:
    Entity() = default;
    Entity(const Entity& other);
    Entity(Entity&& other);
    Entity& operator=(const Entity& other);
    Entity& operator=(Entity&& other);
    virtual ~Entity();

    template <typename T, typename = std::enable_if_t<std::is_base_of_v<Component, T>>>
    void RemoveComponent();
    void RemoveComponent(Component* component);
    void AddComponent(Component* component);

    const std::vector<Component*>& GetComponents() const;

private:
    std::vector<Component*> m_components; // [a_vorontsov] Bad, bad thing...
};

template <typename T, typename>
void Entity::RemoveComponent()
{
    auto it = std::find_if(m_components.begin(), m_components.end(), [](Component* c) { return dynamic_cast<T*>(c) != nullptr });
    if (it != m_components.end())
    {
        delete &(*it);
        m_components.erase(it);
    }
}

inline const std::vector<Component*>& Entity::GetComponents() const
{
    return m_components;
}
}