//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <typeinfo>
#include <string>

namespace Kioto
{

#if _WIN64 && _MSC_VER && !__INTEL_COMPILER
using byte = unsigned char;
using uint8 = unsigned char;
using int16 = short;
using uint16 = unsigned short;
using int32 = int;
using uint32 = unsigned int;
using int64 = long long;
using uint64 = unsigned long long;

using float32 = float;
using float64 = double;
#endif

// [a_vorontsov] Maybe in another header? Include <typeinfo> in every file is not so good idea.
///
/// Get parameter id. (RTTI internally).
///
template <typename T>
uint32_t GetTypeId(const T& t)
{
    const std::type_info& info = typeid(t);
    return info.hash_code();
}
///
/// Get template argument type id. (RTTI internally).
///
template <typename T>
uint32_t GetTypeId()
{
    const std::type_info& info = typeid(T);
    return info.hash_code();
}
///
/// Get parameter name. (RTTI internally).
///
template <typename T>
std::string GetTypeName(const T& t)
{
    const std::type_info& info = typeid(t);
    return info.name();
}
///
/// Get template argument type name. (RTTI internally).
///
template <typename T>
std::string GetTypeName()
{
    const std::type_info& info = typeid(T);
    return info.name();
}

}