//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Systems/RenderSystem.h"

#include "AssetsSystem/AssetsSystem.h"
#include "Component/RenderComponent.h"
#include "Core/ECS/Entity.h"
#include "Render/Geometry/Mesh.h"
#include "Render/Material.h"
#include "Render/RenderObject.h"
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
    m_renderObjects.reserve(2048);
    m_components.reserve(2048);
}

void RenderSystem::Init()
{
    AddRenderPass(new Renderer::ForwardRenderPass());
}

void RenderSystem::OnEntityAdd(Entity* entity)
{
    RenderComponent* rc = entity->GetComponent<RenderComponent>();
    if (rc == nullptr)
        return;

    Renderer::RenderObject* ro = new Renderer::RenderObject();
    std::string material = AssetsSystem::GetAssetFullPath(rc->GetMaterial());
    std::string mesh = AssetsSystem::GetAssetFullPath(rc->GetMesh());
    assert(!material.empty() && !mesh.empty());

    ro->SetMaterial(AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Renderer::Material>(material));
    ro->SetMesh(AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Renderer::Mesh>(mesh));

    rc->SetRenderObject(ro);

    m_components.push_back(rc);
}

void RenderSystem::OnEntityRemove(Entity* entity)
{
    RenderComponent* t = entity->GetComponent<RenderComponent>();
    if (t == nullptr)
        return;
    auto it = std::find(m_components.begin(), m_components.end(), t);
    if (it != m_components.end())
    {
        Renderer::RenderObject* ro = t->GetRenderObject();
        SafeDelete(ro);
        t->SetRenderObject(nullptr);
        m_components.erase(it);
    }
}

void RenderSystem::Update(float32 dt)
{
    for (auto rc : m_components)
    {
        Renderer::RenderObject* ro = rc->GetRenderObject();
        TransformComponent* tc = rc->GetEntity()->GetTransform();
        ro->SetToWorld(tc->GetToWorld());
        ro->SetToModel(tc->GetToModel());
        m_renderObjects.push_back(ro); // [a_vorontcov] TODO: Don't like copying this around.
    }

    for (auto pass : m_renderPasses)
    {
        pass->SetRenderObjects(m_renderObjects);
        pass->Setup();
    }

    for (auto pass : m_renderPasses)
    {
        pass->CollectRenderData();
        pass->Cleanup();
    }
    m_renderObjects.clear();
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