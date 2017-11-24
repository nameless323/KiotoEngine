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
    void operator()(EventPtr e);

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
    void Subscribe(EventType eType, EventCallback callback, void* context = nullptr);
    template <typename EvType, typename = std::enable_if_t<std::is_convertible_v<EvType*, Event*>>>
    void Subscribe(EventCallback callback, void* context = nullptr);

    void Unsubscribe(EventType eType, EventCallback callback);
    template <typename EvType, typename = std::enable_if_t<std::is_convertible_v<EvType*, Event*>>>
    void Unsubscribe(EventCallback callback);
    void Unsubscribe(void* context);

    void Clear();

private:
    struct CallbackWrapper
    {
    public:
        CallbackWrapper(EventCallback callback, void* context)
            : m_callback(callback), m_context(context)
        {}
        void operator()(EventPtr e)
        {
            m_callback(e);
        }
        const EventCallback& GetCallback() const
        {
            return m_callback;
        }
        void* GetContext() const
        {
            return m_context;
        }

    private:
        EventCallback m_callback;
        void* m_context = nullptr;
    };

    std::map<EventType, std::vector<CallbackWrapper>> m_events; // [a_vorontsov] TODO: test on big amount of events for memory and performance. Maybe unordered map will be faster.
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

template <typename EvType, typename>
inline void EventSystem::Subscribe(EventCallback callback, void* context)
{
    Subscribe(typename EvType::GetEventTypeS(), callback, context);
}

template <typename EvType, typename>
inline void EventSystem::Unsubscribe(EventCallback callback)
{
    Unsubscribe(typename EvType::GetEventTypeS(), callback);
}
}