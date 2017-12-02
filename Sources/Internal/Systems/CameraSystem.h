//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include "Core/ECS/SceneSystem.h"

#include <vector>

namespace Kioto
{
class CameraComponent;

class CameraSystem : public SceneSystem
{
public:
    CameraSystem();
    ~CameraSystem() override;

    void OnEntityAdd(Entity* entity) override;
    void OnEntityRemove(Entity* entity) override;
    KIOTO_API void Update(float32 dt) override;

    CameraComponent* GetMainCamera() const;

private:
    void UpdateView(CameraComponent* t);
    void UpdateProjection(CameraComponent* cam);

    CameraComponent* m_mainCamera = nullptr;
    std::vector<CameraComponent*> m_components;
};

inline CameraComponent* CameraSystem::GetMainCamera() const
{
    return m_mainCamera;
}
}