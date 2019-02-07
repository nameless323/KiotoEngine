//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Systems/RenderSystem.h"
#include "Render/Material.h"
#include "Render/RenderPass/ForwardRenderPass.h"

namespace Kioto
{
namespace
{
constexpr uint32 MaxRenderPassesCount = 128;
}

RenderSystem::RenderSystem()
{
    m_renderPasses.reserve(MaxRenderPassesCount);
}

void RenderSystem::Init()
{
    AddRenderPass(new Renderer::ForwardRenderPass());
}

void RenderSystem::OnEntityAdd(Entity* entity)
{
}

void RenderSystem::OnEntityRemove(Entity* entity)
{
}

void RenderSystem::Update(float32 dt)
{
    for (auto pass : m_renderPasses)
        pass->Setup();

    for (auto pass : m_renderPasses)
    {
        pass->CollectRenderData();
        pass->Cleanup();
    }
}

void RenderSystem::Draw()
{
    for (auto pass : m_renderPasses)
    {
        pass->SubmitRenderData();
    }
}

void RenderSystem::Shutdown()
{
    for (auto it : m_renderPasses)
        SafeDelete(it);
    m_renderPasses.clear();
}

void RenderSystem::AddRenderPass(Renderer::RenderPass* pass)
{
    m_renderPasses.push_back(pass);
}

void RenderSystem::RemoveRenderPass(Renderer::RenderPass* pass)
{
    m_renderPasses.erase(std::remove(m_renderPasses.begin(), m_renderPasses.end(), pass), m_renderPasses.end());
    SafeDelete(pass);
}
}