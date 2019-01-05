//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/CoreHelpers.h"

namespace Kioto
{
#define DECLARE_EVENT(type) \
public:\
uint64 GetEventType() const override \
{ \
    return type::GetEventTypeS(); \
} \
static uint64 GetEventTypeS() \
{ \
    static type a; \
    static uint64 res = PtrToUint(&a); \
    return res; \
}

struct Event
{
public:
    Event() = default;
    virtual ~Event() = default;

    virtual uint64 GetEventType() const abstract;
    virtual void* GetEventData() abstract; // [a_vorontcov] TODO:: T* GetEventData() { return reinterpret_cast<data> };
    void* Sender = nullptr;
    float64 Time = -1.0f;
};
}