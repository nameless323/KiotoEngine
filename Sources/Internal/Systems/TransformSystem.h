#pragma once

#include <vector>

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include "Core/ECS/SceneSystem.h"

namespace Kioto
{
class TransformComponent;

class TransformSystem : public SceneSystem
{
public:
    TransformSystem();
    ~TransformSystem() override;

    void OnEntityAdd(Entity* entity) override;
    void OnEntityRemove(Entity* entity) override;
    KIOTO_API void Update(float32 dt) override;

private:
    void ComposeMatricies(TransformComponent* t);

    std::vector<TransformComponent*> mComponents;
};
}