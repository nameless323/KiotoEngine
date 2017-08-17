//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <chrono>
#include <vector>

#include "Core/CoreTypes.h"

namespace Kioto
{
using SteadyClock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<SteadyClock>;

template <typename T>
using Duration = std::chrono::duration<float64, T>;

class PerformanceTimer
{
public:
    PerformanceTimer();
    PerformanceTimer& operator=(const PerformanceTimer& rhs) = delete;
    PerformanceTimer(const PerformanceTimer& rhs) = delete;
    ~PerformanceTimer() = default;

    ///
    /// Timer start;
    ///
    void Start();
    ///
    /// Timer stop. You can use GetDeltaMs() to get time between start and stop;
    ///
    void Stop();
    ///
    /// Write down lap time. Timer should be started. Up to 32 lap values will be cheap. Next it will take time to resize internal vector.
    ///
    void Lap();
    ///
    /// Get milliseconds between timer start and stop (Timer should be stopped).
    ///
    float64 GetDeltaMs();
    ///
    /// Get all established lap delta times from timer start in order you set the laps.
    /// Conversion from duration to double will be performed for each element.
    ///
    std::vector<float64> GetLaps();
    ///
    /// Reset timer data (delta milliseconds from last start/stop and reset all laps.
    ///
    void Reset();

private:
    TimePoint m_start;
    TimePoint m_end;
    std::vector<TimePoint> m_timestamps;
    float64 m_delta = 0;
};

inline PerformanceTimer::PerformanceTimer()
{
    m_timestamps.reserve(32);
    m_start = m_end = SteadyClock::now();
}

inline void PerformanceTimer::Start()
{
    m_start = m_end = SteadyClock::now();
}

inline void PerformanceTimer::Stop()
{
    m_end = SteadyClock::now();
    m_delta = Duration<std::milli>(m_end - m_start).count();
}

inline void PerformanceTimer::Lap()
{
    m_timestamps.emplace_back(std::chrono::steady_clock::now());
}

inline double PerformanceTimer::GetDeltaMs()
{
    return m_delta;
}

inline std::vector<float64> PerformanceTimer::GetLaps()
{
    std::vector<float64> res;
    for (const auto& e : m_timestamps)
        res.emplace_back(Duration<std::milli>(e - m_start).count());
    return res;
}

inline void PerformanceTimer::Reset()
{
    m_timestamps.clear();
    m_start = m_end = SteadyClock::now();
}
}
