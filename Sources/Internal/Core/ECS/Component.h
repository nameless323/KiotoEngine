//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include <functional>

namespace Kioto
{
class Entity;

// [a_vorontsov] TODO: hash component type by name via macro as in events.
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
}

class Component
{
public:
    KIOTO_API virtual ~Component() = default;

    KIOTO_API Entity* GetEntity() const;
    KIOTO_API virtual Component* Clone() const abstract;
    KIOTO_API virtual uint64 GetType() const;

private:
    KIOTO_API inline void SetEntity(Entity* entity);

    Entity* m_entity = nullptr;

    friend class Entity;
};

inline Entity* Component::GetEntity() const
{
    return m_entity;
}

inline void Component::SetEntity(Entity* entity)
{
    m_entity = entity;
}

inline uint64 Component::GetType() const 
{
    return -1;
}
}