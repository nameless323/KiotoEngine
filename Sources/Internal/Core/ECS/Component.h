//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

namespace Kioto
{
class Entity;

class Component
{
public:
    virtual ~Component() = default;

    Entity* GetEntity() const;
    virtual Component* Clone() const abstract;

private:
    inline void SetEntity(Entity* entity);

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