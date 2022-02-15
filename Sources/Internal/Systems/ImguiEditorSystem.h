#pragma once

#include <vector>

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include "Core/ECS/SceneSystem.h"

namespace Kioto
{
class Entity;
class LightComponent;
class RenderComponent;
class TransformComponent;
class CameraComponent;

class ImguiEditorSystem : public SceneSystem
{
public:
    ImguiEditorSystem();
    ~ImguiEditorSystem() override;

    void OnEntityAdd(Entity* entity) override;
    void OnEntityRemove(Entity* entity) override;
    KIOTO_API void Update(float32 dt) override;

private:
    void DrawComponentEditor(LightComponent* lightComponent);
    void DrawComponentEditor(RenderComponent* renderComponent);
    void DrawComponentEditor(TransformComponent* transform);
    void DrawComponentEditor(CameraComponent* cameraComponent);

    std::vector<Entity*> mEntities;
    std::vector<const char*> mEntitiesNames; // [a_vorontcov] meh :(
};
}