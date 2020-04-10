#include "stdafx.h"

#include "Core/ECS/Component.h"

namespace Kioto
{
void Component::SetEntity(Entity* entity)
{
    m_entity = entity;
}
}