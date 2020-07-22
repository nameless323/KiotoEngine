#include "stdafx.h"

#include "Systems/LightSystem.h"

#include "Component/LightComponent.h"
#include "Core/ECS/Entity.h"

#include "Core/Logger/Logger.h"

namespace Kioto
{
LightSystem::LightSystem()
{
    m_lights.reserve(256);
}

LightSystem::~LightSystem()
{
    m_lights.clear();
}

void LightSystem::Init()
{
}

void LightSystem::OnEntityAdd(Entity* entity)
{
    LightComponent* light = entity->GetComponent<LightComponent>();
    if (light == nullptr)
        return;
    m_lights.push_back(light);
}

void LightSystem::OnEntityRemove(Entity* entity)
{
    LightComponent* light = entity->GetComponent<LightComponent>();
    if (light == nullptr)
        return;
    auto it = std::find(m_lights.begin(), m_lights.end(), light);
    if (it != m_lights.end())
        m_lights.erase(it);
}

void LightSystem::Update(float32 dt)
{}

void LightSystem::Shutdown()
{}

}