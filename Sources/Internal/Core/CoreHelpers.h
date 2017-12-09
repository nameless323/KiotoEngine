//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <functional>

namespace Kioto
{
namespace
{
std::hash<std::string> StringHasher;
}

template <typename T>
inline void SafeDelete(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete ptr;
        ptr = nullptr;
    }
    // [a_vorontsov] Anton Smetanin suggest to add some logs here in future.
}

template <typename T>
inline void SafeDeleteArray(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete[] ptr;
        ptr = nullptr;
    }
    // [a_vorontsov] Anton Smetanin suggest to add some logs here in future
}

template <typename T>
inline uint64 PtrToUint(T* ptr)
{
    return static_cast<uint64>(reinterpret_cast<uintptr_t>(ptr));
}

inline uint64 StringToHash(const std::string& str)
{
    return StringHasher(str);
}

// [a_vorontsov] See https://stackoverflow.com/questions/18039723/c-trying-to-get-function-address-from-a-stdfunction for details.
template<typename T, typename... U>
uint64 GetFunctionAddress(std::function<T(U...)> f) 
{
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return PtrToUint(*fnPointer);
}

inline std::string WstrToStr(std::wstring s)
{
    return { s.begin(), s.end() };
}

inline std::wstring StrToWstr(std::string s)
{
    return { s.begin(), s.end() };
}
}