//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <functional>
#include <map>

#include "Core/CoreTypes.h"

namespace Kioto
{
struct Event;
using EventPtr = std::shared_ptr<Event>;
using EventType = uint64;

struct EventCallback
{
public:
    EventCallback(std::function<void(EventPtr)> callback);
    EventCallback(EventCallback&& other);
    EventCallback(const EventCallback& other);
    EventCallback& operator= (EventCallback other);
    ~EventCallback() = default;

    bool operator==(const EventCallback& other) const;
    bool operator!=(const EventCallback& other) const;
    void operator()(std::shared_ptr<Event> e);

    template <typename T, void(T::* callback)(EventPtr)>
    static EventCallback MakeCallback(T* context);

private:
    uint32 m_index = 0;
    std::function<void(EventPtr)> m_callback;
    static uint32 m_currentIndex;

    friend void swap(EventCallback& e1, EventCallback& e2);
};

class EventSystem
{
public:
    EventSystem() = default;
    ~EventSystem() = default;

    void RaiseEvent(EventPtr e);
    void Subscribe(EventType eType, EventCallback callback);
    void Unsubscribe(EventType eType, EventCallback callback);

private:
    std::map<EventType, std::vector<EventCallback>> m_events;
};

template <typename T, void(T::* callback)(EventPtr)>
static EventCallback EventCallback::MakeCallback(T* context)
{
    return EventCallback([&context, c = callback](EventPtr e) { (context->*c)(e); });
}

inline void swap(EventCallback& e1, EventCallback& e2)
{
    using namespace std;
    std::swap(e1.m_index, e2.m_index);
    swap(e1.m_callback, e2.m_callback);
}
}