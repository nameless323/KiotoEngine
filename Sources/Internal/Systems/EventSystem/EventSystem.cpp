//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Systems/EventSystem/EventSystem.h"

#include "Core/CoreHelpers.h"
#include "Systems/EventSystem/Event.h"

namespace Kioto
{

void EventSystem::RaiseEvent(std::shared_ptr<Event> e)
{
    auto it = m_events.find(e->GetEventType());
    if (it != m_events.end())
    {
        for (auto fun : it->second)
            fun(std::move(e));
    }
}

void EventSystem::Subscribe(EventType eType, EventFunction callback)
{
    auto it = m_events.find(eType);
    if (it != m_events.end())
    {
        auto funIt = std::find_if(it->second.begin(), it->second.end(), [&callback](const EventFunction& fun)
        {
            return GetFunctionAddress(fun) == GetFunctionAddress(callback);
        });
        if (funIt != it->second.end())
            return;
    }
    m_events[eType].emplace_back(std::move(callback));
}

void EventSystem::Unsubscribe(EventType eType, EventFunction callback)
{
    auto it = m_events.find(eType);
    if (it != m_events.end())
    {
        auto funIt = std::find_if(it->second.begin(), it->second.end(), [&callback](const EventFunction& fun)
        {
            return GetFunctionAddress(fun) == GetFunctionAddress(callback);
        });
        if (funIt != it->second.end())
            m_events[eType].erase(funIt);
    }
}

}