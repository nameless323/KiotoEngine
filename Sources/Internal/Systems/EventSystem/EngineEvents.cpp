//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

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
    return &m_data;
}

}