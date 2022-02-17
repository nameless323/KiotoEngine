// Example of usage
// class EventTst
// {
// public:
//     void EventFun(std::shared_ptr<Event> ev)
//     {
//         std::string s = static_cast<OnEntityAddEvent::EntityAddData*>(ev->GetEventData())->someString;
//     }
// 
//     EventTst(EventSystem* es)
//     {
//         es->Subscribe<OnEntityAddEvent>(clbk, this);
//         eSystem = es;
//     }
//     ~EventTst()
//     {
//         eSystem->Unsubscribe(this);
//     }
//     EventSystem* eSystem = nullptr;
//     EventCallback clbk = EventCallback::MakeCallback<EventTst, &EventTst::EventFun>(this);
// };
// 
// auto ePtr = std::make_shared<OnEntityAddEvent>();
// ePtr->Time = 12;
// static_cast<OnEntityAddEvent::EntityAddData*>(ePtr->GetEventData())->std = "tst";
// EventSystem es;
// es.RaiseEvent(ePtr);

#pragma once

#include <functional>
#include <map>

#include "Core/CoreTypes.h"

namespace Kioto
{
struct Event;
using EventPtr = std::shared_ptr<Event>;
using EventType = uint64;

/// Describes calback for events. Simple wrapper consists of std::funciton for callback and index for equality operator.
/// You can use static method MakeCallback to create callback from member function, or you can use lambda or free functions or static functions
/// with ctor with std::function. Note: you must ether remember your callback and unsubscribe with this callback, or you can add context in eventSystem.Subscribe
/// (this ptr), and Unsubscribe via eventSystem.Unsubscribe(this). Only callbacks creates via  EventCallback(std::function<void(EventPtr)> callback); or
/// created via MakeCallback (which essentialy the same) are considered unique.
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

    /// Makes Callback from member function.
    /// EventCallback clbk = EventCallback::MakeCallback<ClassName, &ClassName::MemFunName>(this);
    template <typename T, void(T::* callback)(EventPtr)>
    static EventCallback MakeCallback(T* context);

private:
    uint32 mIndex = 0;
    std::function<void(EventPtr)> mCallback;
    static uint32 mCurrentIndex;

    friend void swap(EventCallback& e1, EventCallback& e2);
};

class EventSystem
{
public:
    EventSystem() = default;
    ~EventSystem() = default;

    /// Raise event. Pass std::shared_ptr<Event>, event system will call all subscribed callbacks and pass shared ptr to callbacks.
    void RaiseEvent(EventPtr e);

    /// Subscribe to specific event. If you have this ptr you can pass it to context and you will be able to unsubscribe via Unsubscribe(this).
    void Subscribe(EventType eType, EventCallback callback, void* context = nullptr);

    /// Subscribe to specific event. Pass event type as template arg. If you have "this" ptr you can pass it to context and you will be able to unsubscribe via Unsubscribe(this).
    template <typename EvType, typename = std::enable_if_t<std::is_convertible_v<EvType*, Event*>>>
    void Subscribe(EventCallback callback, void* context = nullptr);

    /// Unsubscribe specific callback from specific event.
    void Unsubscribe(EventType eType, EventCallback callback);

    /// Unsubscribe specific callback from specific event. Pass event type as template arg.
    template <typename EvType, typename = std::enable_if_t<std::is_convertible_v<EvType*, Event*>>>
    void Unsubscribe(EventCallback callback);

    /// Uscubscribe from all events for specific context. See Subscribe doc.
    void Unsubscribe(void* context);

    /// Clear all events.
    void Clear();

    static EventSystem GlobalEventSystem;

private:
    struct CallbackWrapper
    {
    public:
        CallbackWrapper(EventCallback callback, void* context)
            : mCallback(callback), mContext(context)
        {}
        void operator()(EventPtr e)
        {
            mCallback(e);
        }
        const EventCallback& GetCallback() const
        {
            return mCallback;
        }
        void* GetContext() const
        {
            return mContext;
        }

    private:
        EventCallback mCallback;
        void* mContext = nullptr;
    };

    std::map<EventType, std::vector<CallbackWrapper>> mEvents; // [a_vorontcov] TODO: test on big amount of events for memory and performance. Maybe unordered map will be faster.
};

template <typename T, void(T::* callback)(EventPtr)>
static EventCallback EventCallback::MakeCallback(T* context)
{
    return EventCallback([&context, c = callback](EventPtr e) { (context->*c)(e); });
}

inline void swap(EventCallback& e1, EventCallback& e2)
{
    using namespace std;
    std::swap(e1.mIndex, e2.mIndex);
    swap(e1.mCallback, e2.mCallback);
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