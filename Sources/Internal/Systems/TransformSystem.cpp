#include "Systems/TransformSystem.h"

#include "Core/ECS/Entity.h"

namespace Kioto
{
TransformSystem::TransformSystem()
{
    mComponents.reserve(512);
}

TransformSystem::~TransformSystem()
{
    mComponents.clear();
}

void TransformSystem::OnEntityAdd(Entity* entity)
{
    TransformComponent* t = entity->GetTransform();
    if (t == nullptr) // [a_vorontcov] o.O
        return;
    mComponents.push_back(t);
}

void TransformSystem::OnEntityRemove(Entity* entity)
{
    TransformComponent* t = entity->GetTransform();
    if (t == nullptr) // [a_vorontcov] o.O
        return;
    auto it = std::find(mComponents.begin(), mComponents.end(), t);
    if (it != mComponents.end())
        mComponents.erase(it);
}

void TransformSystem::Update(float32 dt)
{
    for (TransformComponent* currTransform : mComponents)
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
    Matrix4 toWorld = t->GetWorldRotation().ToMatrix();
    toWorld.SetTranslation(pos);
    t->SetToWorld(toWorld);

    Matrix4 toModel{};
    assert(toWorld.Inversed(toModel));

    t->SetToModel(toModel);
}
}