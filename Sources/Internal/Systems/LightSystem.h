#pragma once

#include <vector>

#include "Core/CoreTypes.h"
#include "Core/Core.h"

#include "Core/ECS/SceneSystem.h"

namespace Kioto
{
class LightComponent;

class LightSystem : public SceneSystem
{
public:
    LightSystem();
    ~LightSystem() override;

    void Init() override;
    void OnEntityAdd(Entity* entity) override;
    void OnEntityRemove(Entity* entity) override;
    KIOTO_API void Update(float32 dt) override;
    void Shutdown() override;

private:
    std::vector<LightComponent*> m_lights;
};
}