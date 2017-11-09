//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Systems/EventSystem/Event.h"

namespace Kioto
{
struct OnComponentAddEvent : public Event
{
    DECLARE_EVENT(OnComponentAddEvent);

public:
    OnComponentAddEvent() = default;
    ~OnComponentAddEvent() override = default;

    void* GetEventData() override;
};
}