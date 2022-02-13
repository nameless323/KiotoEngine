#include "Core/ECS/Component.h"

namespace Kioto
{
void Component::SetEntity(Entity* entity)
{
    mEntity = entity;
}
}