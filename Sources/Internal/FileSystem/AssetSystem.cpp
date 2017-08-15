//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "FileSystem/AssetSystem.h"

namespace Kioto::AssetSystem
{
using std::wstring;

namespace
{
wstring AssetsPath;
}

void GetAssetsPath()
{
    AssetsPath = L"\\Assets\\"; // [a_vorontsov] TODO: Use post build step in vs, copy assets near the .exe and use GetModuleFileName to get real path. Or smth smarter.
}

wstring GetAssetFullPath(const wstring& relativePath)
{
    if (AssetsPath.empty())
        GetAssetsPath();
    return AssetsPath + relativePath;
}
}
