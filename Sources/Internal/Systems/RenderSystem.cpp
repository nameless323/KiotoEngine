#include "stdafx.h"

#include "Systems/RenderSystem.h"

#include "AssetsSystem/AssetsSystem.h"
#include "Component/LightComponent.h"
#include "Component/RenderComponent.h"
#include "Core/ECS/Entity.h"
#include "Render/Geometry/Mesh.h"
#include "Render/Material.h"
#include "Render/Shader.h"
#include "Render/RenderObject.h"
#include "Render/RenderOptions.h"
#include "Render/RenderPass/ForwardRenderPass.h"
#include "Render/RenderPass/WireframeRenderPass.h"
#include "Render/RenderPass/GrayscaleRenderPass.h"
#include "Render/RenderPass/EditorGizmosPass.h"

namespace Kioto
{
static constexpr uint32 MAX_LIGHTS_COUNT = 256;

RenderSystem::RenderSystem()
{
    m_renderPasses.reserve(Kioto::RenderOptions::MaxRenderPassesCount);
    m_drawData.RenderObjects.reserve(2048);
    m_drawData.Lights.reserve(MAX_LIGHTS_COUNT);
    m_components.reserve(2048);
    m_lights.reserve(256);
}

void RenderSystem::Init()
{
    m_forwardRenderPass = new Renderer::ForwardRenderPass();
    AddRenderPass(m_forwardRenderPass);
    AddRenderPass(new Renderer::EditorGizmosPass());
    AddRenderPass(new Renderer::GrayscaleRenderPass());
    AddRenderPass(new Renderer::WireframeRenderPass());
}

void RenderSystem::OnEntityAdd(Entity* entity)
{
    ParseLights(entity);
    ParseRenderComponents(entity);
}

void RenderSystem::OnEntityRemove(Entity* entity)
{
    TryRemoveLight(entity);
    TryRemoveRenderComponent(entity);
}

void RenderSystem::Update(float32 dt)
{
    for (auto rc : m_components)
    {
        Renderer::RenderObject* ro = rc->GetRenderObject();
        TransformComponent* tc = rc->GetEntity()->GetTransform();
        ro->SetToWorld(tc->GetToWorld());
        ro->SetToModel(tc->GetToModel());
        m_drawData.RenderObjects.push_back(ro); // [a_vorontcov] TODO: Don't like copying this around.
    }
    for (auto l : m_lights)
    {
        l->GetLight()->Position = l->GetEntity()->GetTransform()->GetWorldPosition();
        m_drawData.Lights.push_back(l->GetLight());
    }
    for (auto pass : m_renderPasses)
        m_renderGraph.AddPass(pass);

    m_renderGraph.SheduleGraph();
    m_renderGraph.Execute(m_drawData);
    m_drawData.RenderObjects.clear();
    m_drawData.Lights.clear();
}

void RenderSystem::Draw()
{
    m_renderGraph.Submit();
}

void RenderSystem::Shutdown()
{
    for (auto it : m_renderPasses)
        SafeDelete(it);
    m_renderPasses.clear();
    m_lights.clear();
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

void RenderSystem::ParseLights(Entity* entity)
{
    LightComponent* light = entity->GetComponent<LightComponent>();
    if (light == nullptr)
        return;
    m_lights.push_back(light);
}

void RenderSystem::ParseRenderComponents(Entity* entity)
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

void RenderSystem::TryRemoveLight(Entity* entity)
{
    LightComponent* light = entity->GetComponent<LightComponent>();
    if (light == nullptr)
        return;
    auto it = std::find(m_lights.begin(), m_lights.end(), light);
    if (it != m_lights.end())
        m_lights.erase(it);
}

void RenderSystem::TryRemoveRenderComponent(Entity* entity)
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

}