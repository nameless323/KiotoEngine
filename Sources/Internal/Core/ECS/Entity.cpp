#include "Core/ECS/Entity.h"

#include "Core/CoreHelpers.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{
Entity::Entity(const Entity& other)
{
    for (auto component : other.mComponents)
        AddComponent(component->Clone());
}

Entity::Entity(Entity&& other) noexcept
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
    for (auto& component : mComponents)
        SafeDelete(component);
    mComponents.clear();
}

void Entity::RemoveComponent(Component* component)
{
    auto it = std::find(mComponents.begin(), mComponents.end(), component);
    if (it != mComponents.end())
    {
        delete &(*it);
        mComponents.erase(it);
    }
}

void Entity::AddComponent(Component* component)
{
    if (component->GetEntity() != nullptr)
        return; // [a_vorontcov] TODO: Do something scary here.
    component->SetEntity(this);
    mComponents.push_back(component);
    if (component->GetType() == TransformComponent::GetTypeS())
        mTransform = static_cast<TransformComponent*>(component);
}

Component* Entity::GetComponent(uint64 componentTypeIndex) const
{
    auto it = std::find_if(mComponents.begin(), mComponents.end(), [componentTypeIndex](Component* c) { return c->GetType() == componentTypeIndex; });
    if (it != mComponents.end())
        return *it;
    return nullptr;
}

void Entity::Serialize(YAML::Emitter& out) const
{
    out << YAML::Key << "Entity";
    out << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Name";
    out << YAML::Value << mName;
    out << YAML::Key << "Components";
    out << YAML::Value << YAML::BeginMap;

    for (auto component : mComponents)
    {
        out << YAML::Key << "Component";
        out << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Type";
        out << YAML::Value << component->GetType();
        out << YAML::Comment(component->GetTypeName());
        out << YAML::Key << "Data";
        out << YAML::Value << YAML::BeginMap;

        component->Serialize(out);

        out << YAML::EndMap;
        out << YAML::EndMap;
    }

    out << YAML::EndMap;
    out << YAML::EndMap;
}

void Entity::Deserialize(const YAML::Node& in)
{
    if (in["Name"])
    {
        const std::string name = in["Name"].as<std::string>();
        SetName(name);
    }
    if (in["Components"])
    {
        YAML::Node components = in["Components"];
        for (YAML::const_iterator it = components.begin(); it != components.end(); ++it)
        {
            YAML::Node component = it->second;
            if (component["Type"] && component["Data"])
            {
                const uint64 type = component["Type"].as<uint64>();
                Component* newComponent = ComponentFactory::Instance().CreateComponent(type);
                newComponent->Deserialize(component["Data"]);
                AddComponent(newComponent);
            }
        }
    }
}
}