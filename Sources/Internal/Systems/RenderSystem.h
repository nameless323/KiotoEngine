#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"
#include "Core/ECS/SceneSystem.h"

#include "Render/RenderGraph/RenderGraph.h"
#include "Render/RenderPass/DrawData.h"

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

    std::vector<Renderer::RenderPass*> mRenderPasses;
    std::vector<RenderComponent*> mComponents;
    std::vector<LightComponent*> mLights;

    Renderer::DrawData mDrawData;
    Renderer::ForwardRenderPass* mForwardRenderPass = nullptr;

    Renderer::RenderGraph mRenderGraph;
};
}