//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Core/Timer/GlobalTimer.h"

namespace Kioto::GlobalTimer
{
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::steady_clock::time_point;
using Duration = std::chrono::duration<float>;
using Milliseconds = std::chrono::milliseconds;

namespace
{
TimePoint prevTime;
TimePoint currTime;
float32 dt = 0;
float32 unscaledDt = 0;
float32 timeScale = 1.0f;
float64 timeFromStart = 0;
Clock m_time;
}

void Init()
{
    prevTime = m_time.now();
    currTime = m_time.now();
}

void Tick()
{
    currTime = m_time.now();
    Duration delta = currTime - prevTime;
    prevTime = currTime;

    Milliseconds dtMs = std::chrono::duration_cast<Milliseconds>(delta);
    dt = dtMs.count() * 0.001f;
    timeFromStart += dt;
}

float32 GetDeltaTime()
{
    return dt * timeScale;
}

float64 GlobalTimer::GetTimeFromStart()
{
    return timeFromStart;
}

float32 GlobalTimer::GetUnscaledDt()
{
    return dt;
}

float32 GlobalTimer::GetTimeScale()
{
    return timeScale;
}

void GlobalTimer::SetTimeScale(float32 scale)
{
#if _DEBUG
    if (scale < 0.0f)
    {
        // [a_vorontsov] Some logging or assert here.
    }
#endif
    timeScale = scale;
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
}