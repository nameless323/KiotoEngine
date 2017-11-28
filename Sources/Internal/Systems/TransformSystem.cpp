//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Systems/TransformSystem.h"

#include "Core/ECS/Entity.h"

namespace Kioto
{

void TransformSystem::OnEntityAdd(Entity* entity)
{
    TransformComponent* t = entity->GetTransform();
}

void TransformSystem::OnEntityRemove(Entity* entity)
{

}

void TransformSystem::Update(float32 dt)
{

}

}