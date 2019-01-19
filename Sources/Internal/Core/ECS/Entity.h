//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <vector>

#include "Core/ECS/Component.h"
#include "Component/TransformComponent.h"

namespace Kioto
{
class Entity
{
public:
    KIOTO_API Entity() = default;
    KIOTO_API Entity(const Entity& other);
    KIOTO_API Entity(Entity&& other);
    KIOTO_API Entity& operator=(Entity other);
    KIOTO_API virtual ~Entity();

    template <typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Component*>>>
    void RemoveComponent();
    KIOTO_API void RemoveComponent(Component* component);
    KIOTO_API void AddComponent(Component* component);

    Component* GetComponent(uint64 componentTypeIndex) const;
    template <typename T, typename = std::enable_if_t<std::is_convertible_v<T*, Component*>>>
    T* GetComponent() const;
    TransformComponent* GetTransform() const;
    const std::vector<Component*>& GetComponents() const;
    const std::string& GetName() const;
    void SetName(std::string name);

private:
    std::vector<Component*> m_components; // [a_vorontcov] Bad, bad thing...
    std::string m_name = "Entity";
    TransformComponent* m_transform = nullptr;

    friend void swap(Entity& e1, Entity& e2);
};

template <typename T, typename>
void Entity::RemoveComponent()
{
    auto it = std::find_if(m_components.begin(), m_components.end(), 
        [](Component* c) 
        {
            return typename T::GetTypeS() == c->GetType();
        });
    if (it != m_components.end())
    {
        delete &(*it);
        m_components.erase(it);
    }
}

template <typename T, typename>
T* Entity::GetComponent() const
{
    Component* c = GetComponent(T::GetTypeS());
    if (c != nullptr)
        return static_cast<T*>(c);
    return nullptr;
}

inline const std::vector<Component*>& Entity::GetComponents() const
{
    return m_components;
}

inline void swap(Entity& e1, Entity& e2)
{
    using std::swap;
    swap(e1.m_components, e2.m_components);
}

inline TransformComponent* Entity::GetTransform() const
{
    return m_transform;
}

inline const std::string& Entity::GetName() const
{
    return m_name;
}

inline void Entity::SetName(std::string name)
{
    std::swap(m_name, name);
}
}