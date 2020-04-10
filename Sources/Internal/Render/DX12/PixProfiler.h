#pragma once

#include "pix3.h"
#include "Render/GpuProfiler.h"
#include <d3d12.h>

namespace Kioto::Renderer
{
class PixProfiler
{
public:
    PixProfiler() = default;
    ~PixProfiler() = default;

    template<class... ArgsT>
    void BeginGpuEvent(const std::string& formatString, ArgsT... args);

    void EndGpuEvent();

    template<class... ArgsT>
    void SetMarker(const std::string& formatString, ArgsT... args);

    template<typename T, class... ArgsT>
    void BeginGpuEvent(T* commandList, const std::string& formatString, ArgsT... args);

    template<typename T>
    void EndGpuEvent(T* commandList);

    template<typename T, class... ArgsT>
    void SetMarker(T* commandList, const std::string& formatString, ArgsT... args);
};

template<class... ArgsT>
inline void PixProfiler::BeginGpuEvent(const std::string& formatString, ArgsT... args)
{
}

inline void PixProfiler::EndGpuEvent()
{
}

template<class... ArgsT>
inline void PixProfiler::SetMarker(const std::string& formatString, ArgsT... args)
{
}

template<typename T, class... ArgsT>
inline void PixProfiler::BeginGpuEvent(T* commandList, const std::string& formatString, ArgsT... args)
{
    PIXBeginEvent(commandList, PIX_COLOR(0, 0, 128), formatString.c_str(), std::forward<ArgsT>(args)...);
}

template<typename T>
inline void PixProfiler::EndGpuEvent(T* commandList)
{
    PIXEndEvent(commandList);
}

template<typename T, class... ArgsT>
inline void PixProfiler::SetMarker(T* commandList, const std::string& formatString, ArgsT... args)
{
    PIXSetMarker(commandList, PIX_COLOR(0, 0, 128), formatString.c_str(), std::forward<ArgsT>(args)...);
}
}