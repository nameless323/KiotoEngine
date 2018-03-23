//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/Core.h"
#include "Core/ECS/SceneSystem.h"
#include "Render/Renderer.h"
#include "Render/RenderPass/MainForwardPass.h"
#include "AssetsSystem/AssetsSystem.h"

namespace Kioto
{
class RenderSystem : public SceneSystem
{
public:
    RenderSystem() = default;
    ~RenderSystem() override = default;

    void Init() override;
    void OnEntityAdd(Entity* entity) override;
    void OnEntityRemove(Entity* entity) override;
    KIOTO_API void Update(float32 dt) override;
    void Shutdown() override;

private:
    Renderer::MainForwardRenderPass m_renderPass;
    std::string m_matPath = AssetsSystem::GetAssetFullPath("Materials\\Test.mt");
    Renderer::Material* m_material = nullptr;
    std::string m_meshPath = AssetsSystem::GetAssetFullPath(R"(Models\Teapot.fbx)");
    Mesh* m_mesh = nullptr;
};
}