#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"
#include "Core/ECS/SceneSystem.h"

#include "Render/RenderGraph/RenderGraph.h"

namespace Kioto
{
namespace Renderer
{
class RenderPass;
class RenderObject;
class ForwardRenderPass;
}

class LightComponent;
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
    void ParseLights(Entity* entity);
    void ParseRenderComponents(Entity* entity);

    void TryRemoveLight(Entity* entity);
    void TryRemoveRenderComponent(Entity* entity);

    std::vector<Renderer::RenderPass*> m_renderPasses;
    std::vector<Renderer::RenderObject*> m_renderObjects;
    std::vector<RenderComponent*> m_components;
    std::vector<LightComponent*> m_lights;
    Renderer::ForwardRenderPass* m_forwardRenderPass = nullptr;

    Renderer::RenderGraph m_renderGraph;
};
}