//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/Timer/GlobalTimer.h"

#include "Core/DataStructures/RingArray.h"

namespace Kioto::GlobalTimer
{
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::duration<float>;
using Milliseconds = std::chrono::milliseconds;

namespace
{
TimePoint PrevTime;
TimePoint CurrTime;
float32 Dt = 0;
float32 UnscaledDt = 0;
float32 TimeScale = 1.0f;
float64 TimeFromStart = 0;
Clock Time;

constexpr uint32 SmoothValuesCount = 30;
RingArray<float32, SmoothValuesCount> DtValues;
}

void Init()
{
    PrevTime = Time.now();
    CurrTime = Time.now();
}

void Tick()
{
    CurrTime = Time.now();
    Duration delta = CurrTime - PrevTime;
    PrevTime = CurrTime;

    Milliseconds dtMs = std::chrono::duration_cast<Milliseconds>(delta);
    Dt = dtMs.count() * 0.001f;
    TimeFromStart += Dt;
    DtValues.Add(Dt);
}

float32 GetDeltaTime()
{
    return Dt * TimeScale;
}

float64 GlobalTimer::GetTimeFromStart()
{
    return TimeFromStart;
}

float32 GlobalTimer::GetUnscaledDt()
{
    return Dt;
}

float32 GlobalTimer::GetTimeScale()
{
    return TimeScale;
}

void GlobalTimer::SetTimeScale(float32 scale)
{
#if _DEBUG
    if (scale < 0.0f)
    {
        // [a_vorontsov] Some logging or assert here.
    }
#endif
    TimeScale = scale;
}

SystemTime GlobalTimer::GetSystemTime()
{
#if _WIN32 || _WIN64
    SystemTime st;
    GetSystemTime(&st);
    return st;
#else
    return {}; // [a_vorontsov] TODO
#endif
}

Kioto::float32 GetSmoothDt()
{
    return DtValues.GetAverage();
}

}