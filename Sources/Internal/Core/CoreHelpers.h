#pragma once

#include "Core/WindowsCore.h"

#include <string>
#include <functional>
#include <locale>

#include "Core/CoreTypes.h"

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
    // [a_vorontcov] Maybe add some logs here in future.
}

template <typename T>
inline void SafeDeleteArray(T*& ptr)
{
    if (ptr != nullptr)
    {
        delete[] ptr;
        ptr = nullptr;
    }
    // [a_vorontcov] Maybe add some logs here in future
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

// [a_vorontcov] See https://stackoverflow.com/questions/18039723/c-trying-to-get-function-address-from-a-stdfunction for details.
template<typename T, typename... U>
uint64 GetFunctionAddress(std::function<T(U...)> f) 
{
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return PtrToUint(*fnPointer);
}

inline std::string WstrToStr(std::wstring s)
{
    // [a_vorontcov] Windows specific.
    if (s.empty()) 
        return std::string();
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &s[0], (int)s.size(), NULL, 0, NULL, NULL);
    std::string strTo(sizeNeeded, 0);
    WideCharToMultiByte(CP_UTF8, 0, &s[0], (int)s.size(), &strTo[0], sizeNeeded, NULL, NULL);
    return strTo;
}

inline std::wstring StrToWstr(std::string s)
{
    return { s.begin(), s.end() };
}
}