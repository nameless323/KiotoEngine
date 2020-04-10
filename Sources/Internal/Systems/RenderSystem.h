#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"
#include "Core/ECS/SceneSystem.h"

namespace Kioto
{
namespace Renderer
{
class RenderPass;
class RenderObject;
}

class RenderComponent;

class RenderSystem : public SceneSystem
{
public:
    RenderSystem();
    ~RenderSystem() override = default;

    void Init() override;
    void OnEntityAdd(Entity* entity) override;
    void OnEntityRemove(Entity* entity) override;
    KIOTO_API void Update(float32 dt) override;
    KIOTO_API void Draw();
    void Shutdown() override;

    void AddRenderPass(Renderer::RenderPass* pass);
    void RemoveRenderPass(Renderer::RenderPass* pass);

private:
    std::vector<Renderer::RenderPass*> m_renderPasses;
    std::vector<Renderer::RenderObject*> m_renderObjects;
    std::vector<RenderComponent*> m_components;
};
}