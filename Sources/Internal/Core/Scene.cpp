#include "Core/Scene.h"

#include <debugapi.h>

#include "CoreHelpers.h"
#include "Core/ECS/SceneSystem.h"
#include "Core/ECS/Entity.h"
#include "Core/Timer/GlobalTimer.h"
#include "Systems/CameraSystem.h"
#include "Systems/EventSystem/EventSystem.h"
#include "Systems/DebugSystem.h"
#include "Systems/ImguiEditorSystem.h"
#include "Systems/TransformSystem.h"
#include "Systems/RenderSystem.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{
Scene::Scene(std::string name)
    : mName(name)
{
    // [a_vorontcov] 64 systems are enough for everyone.
    mSystems.reserve(64);
    mEntities.reserve(512);
}

Scene::~Scene()
{
    for (auto system : mSystems)
    {
        system->Shutdown();
        SafeDelete(system);
    }
    mSystems.clear();

    for (auto entity : mEntities)
        SafeDelete(entity);
    mEntities.clear();
}

void Scene::Init()
{
    EventSystem::GlobalEventSystem.Clear();

    TransformSystem* transformSystem = new TransformSystem();
    AddSystemInternal(transformSystem);
    mCameraSystem = new CameraSystem();
    AddSystemInternal(mCameraSystem);
    mRenderSystem = new RenderSystem();
    AddSystemInternal(mRenderSystem);
    AddSystemInternal({ new ImguiEditorSystem() });
    AddSystemInternal({ new DebugSystem() });

    for (auto system : mSystems)
        system->Init();
}

void Scene::Update(float32 dt)
{
    for (auto system : mSystems)
        system->Update(GlobalTimer::GetDeltaTime());
    mRenderSystem->Draw();
}

void Scene::Shutdown()
{
    for (auto system : mSystems)
    {
        system->Shutdown();
        SafeDelete(system);
    }
    mSystems.clear();
    OutputDebugStringA("Shutdown scene");
}

void Scene::AddSystem(SceneSystem* system)
{
    AddSystemInternal(system);
    system->Init();
}

void Scene::RemoveSystem(SceneSystem* system)
{
    auto it = std::find(mSystems.begin(), mSystems.end(), system);
    if (it != mSystems.end())
    {
        (*it)->Shutdown();
        delete &(*it);
        mSystems.erase(it);
    }
}

void Scene::AddEntity(Entity* entity)
{
    mEntities.push_back(entity);
    for (auto system : mSystems)
        system->OnEntityAdd(entity);
}

void Scene::RemoveEntity(Entity* entity)
{
    auto it = std::find(mEntities.begin(), mEntities.end(), entity);
    if (it != mEntities.end())
    {
        mEntities.erase(it);
        for (auto system : mSystems)
            system->OnEntityAdd(entity);
    }
}

Entity* Scene::FindEntity(const std::string& name) const
{
    for (auto entity : mEntities)
    {
        if (entity->GetName() == name)
            return entity;
    }
    return nullptr;
}

void Scene::AddSystemInternal(SceneSystem* system)
{
    mSystems.push_back(system);
}

void Scene::Serialize(YAML::Emitter& out) const
{
    out << YAML::Key << "SceneName";
    out << YAML::Value << mName;

    out << YAML::Key << "Entities";
    out << YAML::Value << YAML::BeginMap;

    for (auto entity : mEntities)
        entity->Serialize(out);

    out << YAML::EndMap;
}

void Scene::Deserialize(const YAML::Node& in)
{
    if (in["Entities"])
    {
        YAML::Node characterType = in["Entities"];
        for (YAML::const_iterator it = characterType.begin(); it != characterType.end(); ++it)
        {
            Entity* e = new Entity();
            e->Deserialize(it->second);
            AddEntity(e);
        }
    }
}

}