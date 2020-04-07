#include "stdafx.h"

#include "Core/DataStructures/RingArray.h"

namespace Kioto::FPSCounter
{
namespace
{
float32 Timer = 0.0f;
uint32 FPS = 0;
uint32 Ticks = 0;

constexpr uint32 SmoothFpsCount = 60;
RingArray<uint32, SmoothFpsCount> FPSValues;
}

Kioto::uint32 GetFPS()
{
    return FPS;
}

Kioto::uint32 GetSmoothFPS()
{
    return FPSValues.GetAverage();
}

void Tick(float32 dt)
{
    Timer += dt;
    ++Ticks;
    if (Timer > 1.0f)
    {
        FPS = static_cast<uint32>(Ticks / Timer);
        Timer = 0.0f;
        Ticks = 0;

        FPSValues.Add(FPS);
    }
}
}