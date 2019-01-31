//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <vector>

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include "Core/ECS/SceneSystem.h"
#include "Render/Camera.h"

namespace Kioto
{
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

    const Renderer::Camera* GetMainCamera() const;

private:
    void UpdateView(CameraComponent* cam);

    Renderer::Camera* m_mainCamera = nullptr;
    std::vector<CameraComponent*> m_components;
};

inline const Renderer::Camera* CameraSystem::GetMainCamera() const
{
    return m_mainCamera;
}
}