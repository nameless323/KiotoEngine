//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include <functional>

namespace YAML
{
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
KIOTO_API static const std::string& GetTypeName() \
{ \
    static std::string name = #type; \
    return name; \
}

#define BEGIN_COMPONENT_SERIALIZARION() \
out << YAML::Key << "Component"; \
out << YAML::Value << GetTypeS(); \
out << YAML::Comment(GetTypeName()); \
out << YAML::Key << "Data"; \
out << YAML::Value << YAML::BeginMap

#define END_COMPONENT_SERIALIZATION() \
out << YAML::EndMap

class Component
{
public:
    KIOTO_API virtual ~Component() = default;

    KIOTO_API Entity* GetEntity() const;
    KIOTO_API virtual Component* Clone() const abstract;
    KIOTO_API virtual uint64 GetType() const;

    virtual void Save(YAML::Emitter& out) const abstract;

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
}
