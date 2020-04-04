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

struct OnEntityAddEvent : public Event
{
    DECLARE_EVENT(OnEntityAddEvent);

public:
    OnEntityAddEvent() = default;
    ~OnEntityAddEvent() override = default;

    void* GetEventData() override;
};

struct OnMainWindowResized : public Event
{
    DECLARE_EVENT(OnMainWindowResized);

public:
    struct Data
    {
        uint32 width;
        uint32 height;
        float32 aspect;
    };

    OnMainWindowResized() = default;
    ~OnMainWindowResized() override = default;

    void* GetEventData() override;

private:
    Data m_data{};
};
}