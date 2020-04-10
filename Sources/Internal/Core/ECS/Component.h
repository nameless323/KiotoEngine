#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"
#include "Core/ECS/ComponentFactory.h"

#include <functional>

namespace YAML
{
class Node;
class Emitter;
}

namespace Kioto
{
class Entity;

#define DECLARE_COMPONENT(type) \
public:\
KIOTO_API uint64 GetType() const override \
{ \
    return type::GetTypeS(); \
} \
KIOTO_API static uint64 GetTypeS() \
{ \
    static std::hash<std::string> stringHasher; \
    static uint64 hash = stringHasher(#type); \
    return hash; \
} \
KIOTO_API const std::string& GetTypeName() const override \
{ \
    static std::string name = type::GetTypeNameS(); \
    return name; \
} \
KIOTO_API static const std::string& GetTypeNameS() \
{ \
    static std::string name = #type; \
    return name; \
}

class Component
{
public:
    KIOTO_API Component() = default;
    KIOTO_API virtual ~Component() = default;

    KIOTO_API Entity* GetEntity() const;
    KIOTO_API virtual Component* Clone() const abstract;
    KIOTO_API virtual uint64 GetType() const;
    KIOTO_API virtual const std::string& GetTypeName() const;

    virtual void Serialize(YAML::Emitter& out) const abstract;
    virtual void Deserialize(const YAML::Node& in) abstract;

protected:
    virtual void SetEntity(Entity* entity);

private:
    Entity* m_entity = nullptr;

    friend class Entity;
};

inline Entity* Component::GetEntity() const
{
    return m_entity;
}

inline uint64 Component::GetType() const 
{
    return -1;
}

inline const std::string& Component::GetTypeName() const
{
    return this->GetTypeName();
}
}
