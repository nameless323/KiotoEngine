//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/Scene.h"

#include "Core/ECS/SceneSystem.h"
#include "Core/ECS/Entity.h"
#include "Core/Timer/GlobalTimer.h"
#include "Systems/CameraSystem.h"
#include "Systems/EventSystem/EventSystem.h"
#include "Systems/TransformSystem.h"
#include "Systems/RenderSystem.h"

namespace Kioto
{

Scene::Scene()
{
    // [a_vorontcov] 64 systems are enough for anyone.
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
    RenderSystem* renderSystem = new RenderSystem();
    AddSystemInternal(renderSystem);

    for (auto system : m_systems)
        system->Init();
}

void Scene::Update(float32 dt)
{
    for (auto system : m_systems)
        system->Update(GlobalTimer::GetDeltaTime());
}

void Scene::Shutdown()
{
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

void Scene::AddSystemInternal(SceneSystem* system)
{
    m_systems.push_back(system);
}

}