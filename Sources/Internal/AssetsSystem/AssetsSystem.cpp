//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "AssetsSystem/AssetsSystem.h"

#include "yaml-cpp/yaml.h"

namespace Kioto::AssetsSystem
{
using std::wstring;

namespace
{
wstring AssetsPath;
}

void GetAssetsPath()
{
    static constexpr uint32 pathSize = 512; // [a_vorontsov] So max path to assets can be only 512 chars which is bad. Handle it later.
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
}

wstring GetAssetFullPath(const wstring& relativePath)
{
    if (AssetsPath.empty())
        GetAssetsPath();
    return AssetsPath + relativePath;
}

void Init()
{
    wstring configPath = GetAssetFullPath(L"AssetsConfig.yaml");
    YAML::Node config = YAML::LoadFile(std::string(configPath.begin(), configPath.end()));
    if (config["enginePath"] != nullptr)
    {
        std::string path = config["enginePath"].as<std::string>();
        std::wstring tmp(path.begin(), path.end());
        tmp += L"\\Assets\\";
        AssetsPath = tmp;
    }
    else
    {
        throw "Assets Config not found. Please read the Readme file.";
    }
}

}
