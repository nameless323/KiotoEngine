#pragma once

#include <string>
#include <vector>

#include "Core/ECS/Component.h"
#include "Component/TransformComponent.h"

namespace YAML 
{
class Node;
}

namespace Kioto
{
class Entity
{
public:
    KIOTO_API Entity() = default;
    KIOTO_API Entity(const Entity& other);
    KIOTO_API Entity(Entity&& other) noexcept;
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

    void Serialize(YAML::Emitter& out) const;
    void Deserialize(const YAML::Node& in);

private:
    std::vector<Component*> mComponents; // [a_vorontcov] Bad, bad thing...
    std::string mName = "Entity";
    TransformComponent* mTransform = nullptr;

    friend void swap(Entity& e1, Entity& e2);
};

template <typename T, typename>
void Entity::RemoveComponent()
{
    auto it = std::find_if(mComponents.begin(), mComponents.end(), 
        [](Component* c) 
        {
            return typename T::GetTypeS() == c->GetType();
        });
    if (it != mComponents.end())
    {
        delete &(*it);
        mComponents.erase(it);
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
    return mComponents;
}

inline void swap(Entity& e1, Entity& e2)
{
    using std::swap;
    swap(e1.mComponents, e2.mComponents);
}

inline TransformComponent* Entity::GetTransform() const
{
    return mTransform;
}

inline const std::string& Entity::GetName() const
{
    return mName;
}

inline void Entity::SetName(std::string name)
{
    std::swap(mName, name);
}
}
