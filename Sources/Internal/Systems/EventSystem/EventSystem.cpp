//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Systems/EventSystem/EventSystem.h"

#include "Core/CoreHelpers.h"
#include "Systems/EventSystem/Event.h"

namespace Kioto
{

uint32 EventCallback::m_currentIndex = 0;
constexpr uint32 EventsInitialCapacity = 128;

EventSystem EventSystem::GlobalEventSystem;

EventCallback::EventCallback(std::function<void(EventPtr)> callback)
{
    m_index = m_currentIndex++;
    m_callback = callback;
}

EventCallback::EventCallback(EventCallback&& other)
{
    swap(*this, other);
}

EventCallback::EventCallback(const EventCallback& other)
{
    m_callback = other.m_callback;
    m_index = other.m_index;
}

bool EventCallback::operator!=(const EventCallback& other) const
{
    return !(*this == other);
}

bool EventCallback::operator==(const EventCallback& other) const
{
    return m_index == other.m_index;
}

EventCallback& EventCallback::operator=(EventCallback other)
{
    swap(*this, other);
    return *this;
}

void EventCallback::operator()(EventPtr e)
{
    if (m_callback != nullptr)
        m_callback(e);
}

void EventSystem::RaiseEvent(EventPtr e)
{
    auto it = m_events.find(e->GetEventType());
    if (it != m_events.end())
    {
        for (auto& fun : it->second)
            fun(e);
    }
}

void EventSystem::Subscribe(EventType eType, EventCallback callback, void* context /*= nullptr*/)
{
    auto it = m_events.find(eType);
    if (it != m_events.end())
    {
        auto funIt = std::find_if(it->second.begin(), it->second.end(), [&callback](const CallbackWrapper& callbackWrapper)
        {
            return callbackWrapper.GetCallback() == callback;
        });
        if (funIt != it->second.end())
            return;
    }
    else
    {
        m_events[eType].reserve(EventsInitialCapacity);
    }
    m_events[eType].emplace_back(callback, context);
}

void EventSystem::Unsubscribe(EventType eType, EventCallback callback)
{
    auto it = m_events.find(eType);
    if (it != m_events.end())
    {
        auto funIt = std::find_if(it->second.begin(), it->second.end(), [&callback](const CallbackWrapper& callbackWrapper)
        {
            return callbackWrapper .GetCallback()== callback;
        });
        if (funIt != it->second.end())
            m_events[eType].erase(funIt);
    }
}

void EventSystem::Unsubscribe(void* context)
{
    if (context == nullptr)
        return;

    for (auto& et : m_events)
    {
        auto& vec = et.second;
        vec.erase(std::remove_if(vec.begin(), vec.end(), [context](const CallbackWrapper& cw) { return cw.GetContext() == context; }), vec.end());
    }
}

void EventSystem::Clear()
{
    m_events.clear();
}

}