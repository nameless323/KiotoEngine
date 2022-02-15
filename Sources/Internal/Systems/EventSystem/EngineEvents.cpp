#include "Systems/EventSystem/EngineEvents.h"

namespace Kioto
{
void* OnComponentAddEvent::GetEventData()
{
    return nullptr;
}

void* OnEntityAddEvent::GetEventData()
{
    return nullptr;
}

void* OnMainWindowResized::GetEventData()
{
    return &mData;
}
}