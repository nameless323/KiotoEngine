//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Systems/RenderSystem.h"

namespace Kioto
{

void RenderSystem::Init()
{

}

void RenderSystem::OnEntityAdd(Entity* entity)
{

}

void RenderSystem::OnEntityRemove(Entity* entity)
{

}

void RenderSystem::Update(float32 dt)
{
    for (int32 i = 0; i < Renderer::MaxRenderTargetsCount; ++i)
        m_renderPass.SetRenderTarget(i, Renderer::InvalidHandle);
    m_renderPass.SetDepthStencil(Renderer::InvalidHandle);

    m_renderPass.SetViewport({ 0, 0, Renderer::GetWidth(), Renderer::GetHeight() });
    m_renderPass.SetScissor({ 0, 0, Renderer::GetWidth(), Renderer::GetHeight() });
    m_renderPass.SetClearDepth(true);
    m_renderPass.SetClearDepthValue(0.0f);
    m_renderPass.SetClearColor(true);
    m_renderPass.SetClearStencil(true);
    m_renderPass.SetClearStencilValue(0);

    Renderer::AddRenderPass(m_renderPass);
}

void RenderSystem::Shutdown()
{

}

}