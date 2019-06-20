//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Systems/TransformSystem.h"

#include "Core/ECS/Entity.h"

namespace Kioto
{
TransformSystem::TransformSystem()
{
    m_components.reserve(512);
}

TransformSystem::~TransformSystem()
{
    m_components.clear();
}

void TransformSystem::OnEntityAdd(Entity* entity)
{
    TransformComponent* t = entity->GetTransform();
    if (t == nullptr) // [a_vorontcov] o.O
        return;
    m_components.push_back(t);
}

void TransformSystem::OnEntityRemove(Entity* entity)
{
    TransformComponent* t = entity->GetTransform();
    if (t == nullptr) // [a_vorontcov] o.O
        return;
    auto it = std::find(m_components.begin(), m_components.end(), t);
    if (it != m_components.end())
        m_components.erase(it);
}

void TransformSystem::Update(float32 dt)
{
    for (TransformComponent* currTransform : m_components)
    {
        if (currTransform->GetDirty())
        {
            ComposeMatricies(currTransform);
            currTransform->RemoveDirty();
        }
    }
}

void TransformSystem::ComposeMatricies(TransformComponent* t)
{
    const Vector3& pos = t->GetWorldPosition();
    Matrix4 toWorld = t->GetWorldRotation();
    toWorld.SetTranslation(pos);
    t->SetToWorld(toWorld);

    Matrix4 toModel{};
    assert(toWorld.Inversed(toModel));

    t->SetToModel(toModel);
}
}