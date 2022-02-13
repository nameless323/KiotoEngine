#pragma once

#include <chrono>
#include <Windows.h>

#include "Core/CoreTypes.h"

namespace Kioto::GlobalTimer
{
#if _WIN32 || _WIN64
using SystemTime = SYSTEMTIME;
#else
struct SystemTime
{
    float32 wYear;
    float32 wMonth;
    float32 wDayOfWeek;
    float32 wDay;
    float32 wHour;
    float32 wMinute;
    float32 wSecond;
    float32 wMilliseconds;
};
#endif
///
/// Start timer.
///
void Init();
///
/// Tick timer. Call this at every frame.
///
void Tick();
///
/// Get time between frames affected by time scale.
///
float32 GetDeltaTime();
///
/// Get time from timer startup.
///
float64 GetTimeFromStart();
///
/// Get time between frames unaffected by time scale.
///
float32 GetUnscaledDt();
///
/// Get current time scale.
///
float32 GetTimeScale();
///
/// Set time scale.
///
void SetTimeScale(float32 scale);
///
/// Get system time.
///
SystemTime GetSystemTime();
///
/// Get smooth dt averaged by 30 frames.
///
float32 GetSmoothDt();
}