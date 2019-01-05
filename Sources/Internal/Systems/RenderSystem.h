//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"
#include "Core/ECS/SceneSystem.h"
#include "Render/Renderer.h"
#include "Render/RenderPass/ForwardRenderPass.h"
#include "AssetsSystem/AssetsSystem.h"

namespace Kioto
{

constexpr uint32 MaxRenderPassesCount = 128;

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

};

RenderSystem::RenderSystem()
{
    m_renderPasses.reserve(MaxRenderPassesCount);
}
}