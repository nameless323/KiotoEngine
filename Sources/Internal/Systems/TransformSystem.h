//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

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

    std::vector<TransformComponent*> m_components;
};
}