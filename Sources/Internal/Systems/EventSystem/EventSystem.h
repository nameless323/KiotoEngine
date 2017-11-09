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
using EventFunction = std::function<void(std::shared_ptr<Event>)>;
using EventType = uint64;

class EventSystem
{
public:
    EventSystem() = default;
    ~EventSystem() = default;

    void RaiseEvent(std::shared_ptr<Event> e);
    void Subscribe(EventType eType, EventFunction callback);
    void Unsubscribe(EventType eType, EventFunction callback);

private:
    std::map<EventType, std::vector<EventFunction>> m_events;
};
}