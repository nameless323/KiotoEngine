#include "Systems/CameraSystem.h"

#include "Component/CameraComponent.h"
#include "Core/ECS/Entity.h"
#include "Math/MathHelpers.h"
#include "Systems/EventSystem/EventSystem.h"
#include "Systems/EventSystem/EngineEvents.h"
#include "Render/Renderer.h"

namespace Kioto
{
CameraSystem::CameraSystem()
{
    mComponents.reserve(4);
}

CameraSystem::~CameraSystem()
{
    mComponents.clear();
}

void CameraSystem::Init()
{
    EventSystem::GlobalEventSystem.Subscribe<OnMainWindowResized>({
        [this](std::shared_ptr<Event> e)
    {
        if (mMainCamera == nullptr)
            return;
        OnMainWindowResized::Data* resizeData = reinterpret_cast<OnMainWindowResized::Data*>(e->GetEventData());
        mMainCamera->SetAspect(resizeData->aspect);
    }
    }, this);
}

void CameraSystem::OnEntityAdd(Entity* entity)
{
    CameraComponent* t = entity->GetComponent<CameraComponent>();
    if (t == nullptr)
        return;
    mComponents.push_back(t);
}

void CameraSystem::OnEntityRemove(Entity* entity)
{
    CameraComponent* t = entity->GetComponent<CameraComponent>();
    if (t == nullptr)
        return;
    auto it = std::find(mComponents.begin(), mComponents.end(), t);
    if (it != mComponents.end())
        mComponents.erase(it);
}

void CameraSystem::Update(float32 dt)
{
    for (CameraComponent* camComponent : mComponents)
    {
        Renderer::Camera& currCam = camComponent->GetCamera();
        if (camComponent->GetIsMain())
            mMainCamera = &currCam;

        UpdateView(camComponent);

        if (currCam.GetIsProjectionDirty())
            currCam.UpdateProjectionMatrix();

        currCam.UpdateViewProjectionMatrix();
        currCam.UpdateConstantBuffer(); // [a_vorontcov] TODO: Do in when something was really changed.
    }
    Renderer::SetMainCamera(mMainCamera);
}

void CameraSystem::Shutdown()
{
    EventSystem::GlobalEventSystem.Unsubscribe(this);
}

void CameraSystem::UpdateView(CameraComponent* cam)
{
    Matrix4 m = cam->GetTransform()->GetToWorld();
    cam->GetCamera().SetView(m.InversedOrthonorm());
    cam->GetCamera().SetToWorld(m);
}
}