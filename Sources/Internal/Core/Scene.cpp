#include "stdafx.h"

#include "Core/Scene.h"

#include "Core/ECS/SceneSystem.h"
#include "Core/ECS/Entity.h"
#include "Core/Timer/GlobalTimer.h"
#include "Systems/CameraSystem.h"
#include "Systems/EventSystem/EventSystem.h"
#include "Systems/DebugSystem.h"
#include "Systems/TransformSystem.h"
#include "Systems/RenderSystem.h"
#include "Systems/LightSystem.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{
Scene::Scene(std::string name)
    : m_name(name)
{
    // [a_vorontcov] 64 systems are enough for everyone.
    m_systems.reserve(64);
    m_entities.reserve(512);
}

Scene::~Scene()
{
    for (auto system : m_systems)
    {
        system->Shutdown();
        SafeDelete(system);
    }
    m_systems.clear();

    for (auto entity : m_entities)
        SafeDelete(entity);
    m_entities.clear();
}

void Scene::Init()
{
    EventSystem::GlobalEventSystem.Clear();

    TransformSystem* transformSystem = new TransformSystem();
    AddSystemInternal(transformSystem);
    m_cameraSystem = new CameraSystem();
    AddSystemInternal(m_cameraSystem);
    m_lightSystem = new LightSystem();
    AddSystemInternal(m_lightSystem);
    m_renderSystem = new RenderSystem();
    AddSystemInternal(m_renderSystem);
    AddSystemInternal({ new DebugSystem() });

    for (auto system : m_systems)
        system->Init();
}

void Scene::Update(float32 dt)
{
    for (auto system : m_systems)
        system->Update(GlobalTimer::GetDeltaTime());
    m_renderSystem->Draw();
}

void Scene::Shutdown()
{
    for (auto system : m_systems)
    {
        system->Shutdown();
        SafeDelete(system);
    }
    m_systems.clear();
    OutputDebugStringA("Shutdown scene");
}

void Scene::AddSystem(SceneSystem* system)
{
    AddSystemInternal(system);
    system->Init();
}

void Scene::RemoveSystem(SceneSystem* system)
{
    auto it = std::find(m_systems.begin(), m_systems.end(), system);
    if (it != m_systems.end())
    {
        (*it)->Shutdown();
        delete &(*it);
        m_systems.erase(it);
    }
}

void Scene::AddEntity(Entity* entity)
{
    m_entities.push_back(entity);
    for (auto system : m_systems)
        system->OnEntityAdd(entity);
}

void Scene::RemoveEntity(Entity* entity)
{
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end())
    {
        m_entities.erase(it);
        for (auto system : m_systems)
            system->OnEntityAdd(entity);
    }
}

Entity* Scene::FindEntity(const std::string& name) const
{
    for (auto entity : m_entities)
    {
        if (entity->GetName() == name)
            return entity;
    }
    return nullptr;
}

void Scene::AddSystemInternal(SceneSystem* system)
{
    m_systems.push_back(system);
}

void Scene::Serialize(YAML::Emitter& out) const
{
    out << YAML::Key << "SceneName";
    out << YAML::Value << m_name;

    out << YAML::Key << "Entities";
    out << YAML::Value << YAML::BeginMap;

    for (auto entity : m_entities)
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