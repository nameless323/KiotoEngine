//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/Scene.h"

#include "Core/Timer/GlobalTimer.h"
#include "Core/ECS/SceneSystem.h"

namespace Kioto
{

Scene::Scene()
{
    // [a_vorontsov] 64 systems are enough for anyone.
    m_systems.reserve(64);
}

Scene::~Scene()
{
    for (auto system : m_systems)
        SafeDelete(system);
    m_systems.clear();
}

void Scene::Init()
{
    OutputDebugStringA("Init scene");
}

void Scene::Update(float32 dt)
{
    OutputDebugStringA("Update scene\n");
    for (auto system : m_systems)
        system->Update(GlobalTimer::GetDeltaTime());
}

void Scene::Shutdown()
{
    OutputDebugStringA("Shutdown scene");
}

void Scene::AddSystem(SceneSystem* system)
{
    m_systems.push_back(system);
}

void Scene::RemoveSystem(SceneSystem* system)
{
    auto it = std::find(m_systems.begin(), m_systems.end(), system);
    if (it != m_systems.end())
    {
        delete &(*it);
        m_systems.erase(it);
    }
}

}