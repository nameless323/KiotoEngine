#include "Systems/EventSystem/EventSystem.h"

#include "Core/CoreHelpers.h"
#include "Systems/EventSystem/Event.h"

namespace Kioto
{
uint32 EventCallback::mCurrentIndex = 0;
constexpr uint32 EventsInitialCapacity = 128;

EventSystem EventSystem::GlobalEventSystem;

EventCallback::EventCallback(std::function<void(EventPtr)> callback)
{
    mIndex = mCurrentIndex++;
    mCallback = callback;
}

EventCallback::EventCallback(EventCallback&& other)
{
    swap(*this, other);
}

EventCallback::EventCallback(const EventCallback& other)
{
    mCallback = other.mCallback;
    mIndex = other.mIndex;
}

bool EventCallback::operator!=(const EventCallback& other) const
{
    return !(*this == other);
}

bool EventCallback::operator==(const EventCallback& other) const
{
    return mIndex == other.mIndex;
}

EventCallback& EventCallback::operator=(EventCallback other)
{
    swap(*this, other);
    return *this;
}

void EventCallback::operator()(EventPtr e)
{
    if (mCallback != nullptr)
        mCallback(e);
}

void EventSystem::RaiseEvent(EventPtr e)
{
    auto it = mEvents.find(e->GetEventType());
    if (it != mEvents.end())
    {
        for (auto& fun : it->second)
            fun(e);
    }
}

void EventSystem::Subscribe(EventType eType, EventCallback callback, void* context /*= nullptr*/)
{
    auto it = mEvents.find(eType);
    if (it != mEvents.end())
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
        mEvents[eType].reserve(EventsInitialCapacity);
    }
    mEvents[eType].emplace_back(callback, context);
}

void EventSystem::Unsubscribe(EventType eType, EventCallback callback)
{
    auto it = mEvents.find(eType);
    if (it != mEvents.end())
    {
        auto funIt = std::find_if(it->second.begin(), it->second.end(), [&callback](const CallbackWrapper& callbackWrapper)
        {
            return callbackWrapper .GetCallback()== callback;
        });
        if (funIt != it->second.end())
            mEvents[eType].erase(funIt);
    }
}

void EventSystem::Unsubscribe(void* context)
{
    if (context == nullptr)
        return;

    for (auto& et : mEvents)
    {
        auto& vec = et.second;
        vec.erase(std::remove_if(vec.begin(), vec.end(), [context](const CallbackWrapper& cw) { return cw.GetContext() == context; }), vec.end());
    }
}

void EventSystem::Clear()
{
    mEvents.clear();
}
}