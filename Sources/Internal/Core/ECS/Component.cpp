//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/ECS/Component.h"

namespace Kioto
{
void Component::SetEntity(Entity* entity)
{
    m_entity = entity;
}
}