//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"

namespace Kioto
{
class Entity;

class Component
{
public:
    KIOTO_API virtual ~Component() = default;

    KIOTO_API Entity* GetEntity() const;
    KIOTO_API virtual Component* Clone() const abstract;

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
}