//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "AssetsSystem/AssetsSystem.h"

namespace Kioto::AssetsSystem
{
using std::wstring;

namespace
{
wstring AssetsPath;
}

void GetAssetsPath()
{
    //AssetsPath = L"\\Assets\\";
#if 1 // [a_vorontsov] The code below works fine. Just for debugging reasons throw it from build and use relative path.
    static constexpr uint32 pathSize = 512;
    WCHAR path[pathSize];
    DWORD size = GetModuleFileName(nullptr, path, pathSize);
    if (size == 0 || size == pathSize)
    {
        throw std::exception(); // [a_vorontsov] Method failed or path was truncated.
    }

    WCHAR* lastSlash = wcsrchr(path, L'\\');
    if (lastSlash)
    {
        *(lastSlash + 1) = L'\0';
    }
    AssetsPath = path;
    AssetsPath += L"Assets\\";

    wstring ws = AssetsPath;
#endif
}

wstring GetAssetFullPath(const wstring& relativePath)
{
    if (AssetsPath.empty())
        GetAssetsPath();
    return AssetsPath + relativePath;
}
}
