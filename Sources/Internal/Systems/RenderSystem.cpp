#include "Systems/RenderSystem.h"

#include "AssetsSystem/AssetsSystem.h"
#include "Component/LightComponent.h"
#include "Component/RenderComponent.h"
#include "Core/ECS/Entity.h"
#include "Render/Geometry/Mesh.h"
#include "Render/Material.h"
#include "Render/Shader.h"
#include "Render/RenderObject.h"
#include "Render/RenderSettings.h"
#include "Render/RenderPass/ForwardRenderPass.h"
#include "Render/RenderPass/WireframeRenderPass.h"
#include "Render/RenderPass/GrayscaleRenderPass.h"
#include "Render/RenderPass/EditorGizmosPass.h"
#include "Render/RenderPass/ShadowMapRenderPass.h"

namespace Kioto
{
static constexpr uint32 MAX_LIGHTS_COUNT = 256;

RenderSystem::RenderSystem()
{
    mRenderPasses.reserve(Kioto::RenderSettings::MaxRenderPassesCount);
    mDrawData.RenderObjects.reserve(2048);
    mDrawData.Lights.reserve(MAX_LIGHTS_COUNT);
    mComponents.reserve(2048);
    mLights.reserve(256);
}

void RenderSystem::Init()
{
    AddRenderPass(new Renderer::ShadowMapRenderPass);
    mForwardRenderPass = new Renderer::ForwardRenderPass();
    AddRenderPass(mForwardRenderPass);
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
    for (auto rc : mComponents)
    {
        if (!rc->GetIsEnabled())
            continue;
        Renderer::RenderObject* ro = rc->GetRenderObject();
        TransformComponent* tc = rc->GetEntity()->GetTransform();
        ro->SetToWorld(tc->GetToWorld());
        ro->SetToModel(tc->GetToModel());
        mDrawData.RenderObjects.push_back(ro); // [a_vorontcov] TODO: Don't like copying this around.
    }
    for (auto l : mLights)
    {
        if (!l->GetIsEnabled())
            continue;
        l->GetLight()->Position = l->GetEntity()->GetTransform()->GetWorldPosition();
        mDrawData.Lights.push_back(l->GetLight());
    }
    for (auto pass : mRenderPasses)
        mRenderGraph.AddPass(pass);

    mRenderGraph.SheduleGraph();
    mRenderGraph.Execute(mDrawData);
    mDrawData.RenderObjects.clear();
    mDrawData.Lights.clear();
}

void RenderSystem::Draw()
{
    mRenderGraph.Submit();
}

void RenderSystem::Shutdown()
{
    for (auto it : mRenderPasses)
        SafeDelete(it);
    mRenderPasses.clear();
    mLights.clear();
}

void RenderSystem::AddRenderPass(Renderer::RenderPass* pass)
{
    mRenderPasses.push_back(pass);
}

void RenderSystem::RemoveRenderPass(Renderer::RenderPass* pass)
{
    mRenderPasses.erase(std::remove(mRenderPasses.begin(), mRenderPasses.end(), pass), mRenderPasses.end());
    SafeDelete(pass);
}

void RenderSystem::ParseLights(Entity* entity)
{
    LightComponent* light = entity->GetComponent<LightComponent>();
    if (light == nullptr)
        return;
    mLights.push_back(light);
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

    mComponents.push_back(renderComponent);
}

void RenderSystem::TryRemoveLight(Entity* entity)
{
    LightComponent* light = entity->GetComponent<LightComponent>();
    if (light == nullptr)
        return;
    auto it = std::find(mLights.begin(), mLights.end(), light);
    if (it != mLights.end())
        mLights.erase(it);
}

void RenderSystem::TryRemoveRenderComponent(Entity* entity)
{
    RenderComponent* t = entity->GetComponent<RenderComponent>();
    if (t == nullptr)
        return;
    auto it = std::find(mComponents.begin(), mComponents.end(), t);
    if (it != mComponents.end())
    {
        Renderer::RenderObject* ro = t->GetRenderObject();
        SafeDelete(ro);
        t->SetRenderObject(nullptr);
        mComponents.erase(it);
    }
}

}