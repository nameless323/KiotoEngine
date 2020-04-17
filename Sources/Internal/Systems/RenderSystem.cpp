#include "stdafx.h"

#include "Systems/RenderSystem.h"

#include "AssetsSystem/AssetsSystem.h"
#include "Component/RenderComponent.h"
#include "Core/ECS/Entity.h"
#include "Render/Geometry/Mesh.h"
#include "Render/Material.h"
#include "Render/Shader.h"
#include "Render/RenderObject.h"
#include "Render/RenderPass/ForwardRenderPass.h"
#include "Render/RenderPass/WireframeRenderPass.h"

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
    AddRenderPass(new Renderer::WireframeRenderPass());
}

void RenderSystem::OnEntityAdd(Entity* entity)
{
    RenderComponent* renderComponent = entity->GetComponent<RenderComponent>();
    if (renderComponent == nullptr)
        return;

    Renderer::RenderObject* ro = new Renderer::RenderObject();
    std::string materialName = AssetsSystem::GetAssetFullPath(renderComponent->GetMaterial());
    std::string meshName = AssetsSystem::GetAssetFullPath(renderComponent->GetMesh());
    assert(!materialName.empty() && !meshName.empty());

    Renderer::Material* material = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Renderer::Material>(materialName);
    ro->SetMaterial(material);
    ro->SetMesh(AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Renderer::Mesh>(meshName));

    Renderer::RegisterRenderObject(*ro);

    renderComponent->SetRenderObject(ro);

    m_components.push_back(renderComponent);
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