#pragma once

#include <vector>

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include "Core/ECS/SceneSystem.h"

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

    const Renderer::Camera* GetMainCamera() const;

private:
    void UpdateView(CameraComponent* cam);

    Renderer::Camera* mMainCamera = nullptr;
    std::vector<CameraComponent*> mComponents;
};

inline const Renderer::Camera* CameraSystem::GetMainCamera() const
{
    return mMainCamera;
}
}