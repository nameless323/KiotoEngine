//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/CoreHelpers.h"

namespace Kioto
{
#define DECLARE_EVENT(type) \
public:\
uint64 GetEventType() override \
{ \
    return type::GetEventTypeS(); \
} \
static uint64 GetEventTypeS() \
{ \
    static type a; \
    static uint64 res = PtrToUint(&a); \
    return res; \
}

class Event
{
public:
    Event() = default;
    virtual ~Event() = default;

    virtual uint64 GetEventType() abstract;
    virtual void* GetEventData() abstract;
    void* Sender = nullptr;
    float64 Time = -1.0f;
};
}