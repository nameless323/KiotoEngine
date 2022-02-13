#pragma once

#include <sstream>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
#endif

#include <Windows.h> // [a_vorontcov] Must be included before <debugapi.h> to escape super annoying "No target architecture" error.
#include <debugapi.h>

namespace Kioto::Logger
{
namespace Internal
{
inline std::string Separator = "";

template<typename TF>
inline void WriteLog(std::stringstream& ss, const TF& f)
{
    ss << f << std::endl;
    OutputDebugStringA(ss.str().c_str());
}

template<typename TF, typename ... TR>
inline void WriteLog(std::stringstream& ss, const TF& f, const TR& ... rest)
{
    ss << f << Separator;
    WriteLog(ss, rest ...);
}

template<typename ... TR>
inline void WriteLog(const char* file, int line, const TR& ... rest)
{
    std::stringstream ss;
    ss << file << "(" << line << "): " << " | ";
    WriteLog(ss, rest...);
}
}

inline void SetSeparator(std::string separator)
{
    Internal::Separator = std::move(separator);
}

inline void ResetSeparator()
{
    Internal::Separator = "";
}
}

#define LOG(...) Kioto::Logger::Internal::WriteLog(__FILE__, __LINE__, __VA_ARGS__)