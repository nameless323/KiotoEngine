#pragma once

#include <vector>

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include "Core/ECS/SceneSystem.h"

namespace Kioto
{
class Entity;

class ImguiEditorSystem : public SceneSystem
{
public:
    ImguiEditorSystem();
    ~ImguiEditorSystem() override;

    void OnEntityAdd(Entity* entity) override;
    void OnEntityRemove(Entity* entity) override;
    KIOTO_API void Update(float32 dt) override;

private:
    std::vector<Entity*> m_entities;
    std::vector<const char*> m_entitiesNames; // [a_vorontcov] meh :(
};
}