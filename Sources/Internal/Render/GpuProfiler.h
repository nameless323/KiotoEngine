#pragma once

#include <string>

namespace Kioto::Renderer
{
template <typename ConcreteProfiler>
class GpuProfiler final
{
public:
    GpuProfiler() = default;
    ~GpuProfiler() = default;

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

private:
    ConcreteProfiler mProfiler;
};

template <typename ConcreteProfiler>
template<class... ArgsT>
inline void GpuProfiler<ConcreteProfiler>::BeginGpuEvent(const std::string& formatString, ArgsT... args)
{
    mProfiler.BeginGpuEvent(formatString, std::forward<ArgsT>(args)...);
}

template <typename ConcreteProfiler>
inline void GpuProfiler<ConcreteProfiler>::EndGpuEvent()
{
    mProfiler.EndGpuEvent();
}

template <typename ConcreteProfiler>
template<class... ArgsT>
inline void GpuProfiler<ConcreteProfiler>::SetMarker(const std::string& formatString, ArgsT... args)
{
    mProfiler.SetMarker(formatString, std::forward<ArgsT>(args)...);
}

template <typename ConcreteProfiler>
template<typename T, class... ArgsT>
inline void GpuProfiler<ConcreteProfiler>::BeginGpuEvent(T* commandList, const std::string& formatString, ArgsT... args)
{
    mProfiler.BeginGpuEvent(commandList, formatString, std::forward<ArgsT>(args)...);
}

template <typename ConcreteProfiler>
template<typename T>
inline void GpuProfiler<ConcreteProfiler>::EndGpuEvent(T* commandList)
{
    mProfiler.EndGpuEvent(commandList);
}

template <typename ConcreteProfiler>
template<typename T, class... ArgsT>
inline void GpuProfiler<ConcreteProfiler>::SetMarker(T* commandList, const std::string& formatString, ArgsT... args)
{
    mProfiler.SetMarker(commandList, formatString, std::forward<ArgsT>(args)...);
}
}