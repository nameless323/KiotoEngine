//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include "Core/ECS/SceneSystem.h"

#include <vector>

namespace Kioto
{
namespace Renderer
{
class Camera;
}

class CameraComponent;
class EventSystem;

class CameraSystem : public SceneSystem
{
public:
    CameraSystem();
    ~CameraSystem() override;

    void Init() override;
    void OnEntityAdd(Entity* entity) override;
    void OnEntityRemove(Entity* entity) override;
    KIOTO_API void Update(float32 dt) override;
    void Shutdown() override;

    Renderer::Camera* GetMainCamera() const;

private:
    void UpdateView(CameraComponent* t);
    void UpdateProjection(Renderer::Camera* cam);

    Renderer::Camera m_mainCamera;
    std::vector<CameraComponent*> m_components;
};

inline Renderer::Camera* CameraSystem::GetMainCamera() const
{
    return &m_mainCamera;
}
}