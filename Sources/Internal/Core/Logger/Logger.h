//
// Copyright (C) Aleksandr Vorontcov. 2019
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <sstream>

#include <debugapi.h>

namespace Logger
{
namespace Internal
{
std::string Separator = "";

template<typename TF>
void WriteLog(std::stringstream& ss, const TF& f)
{
    ss << f << std::endl;
    OutputDebugStringA(ss.str().c_str());
}

template<typename TF, typename ... TR>
void WriteLog(std::stringstream& ss, const TF& f, const TR& ... rest)
{
    ss << f << Separator;
    WriteLog(ss, rest ...);
}

template<typename ... TR>
void WriteLog(const char* file, int line, const TR& ... rest)
{
    std::stringstream ss;
    ss << file << "(" << line << ")" << " | ";
    WriteLog(ss, rest...);
}
}

void SetSeparator(std::string separator)
{
    Internal::Separator = std::move(separator);
}

void ResetSeparator()
{
    Internal::Separator = "";
}
}

#define LOG(...) Logger::Internal::WriteLog(__FILE__, __LINE__, __VA_ARGS__)