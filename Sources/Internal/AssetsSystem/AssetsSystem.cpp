//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include <fstream>

#include "AssetsSystem/AssetsSystem.h"
#include "Core/CoreHelpers.h"
#include "Render/Material.h"

#include "yaml-cpp/yaml.h"

namespace Kioto::AssetsSystem
{
using std::wstring;
using std::string;

namespace
{
string AssetsPath;

static RenderAssetsManager<Renderer::Texture> m_textureManager;
static RenderAssetsManager<Renderer::Shader> m_shaderManager;
static RenderAssetsManager<Renderer::Material> m_materialManager;
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
    AssetsPath = WstrToStr(path);
    AssetsPath += "Assets\\";
}

std::string GetAssetFullPath(const std::string& assetName)
{
    if (AssetsPath.empty())
        GetAssetsPath();
    return AssetsPath + assetName;
}

void Init()
{
#if _DEBUG
    string configPath = GetAssetFullPath("AssetsConfig.yaml");
    if (!CheckIfFileExist(configPath))
        throw "Assets Config not found. Please read the Readme file.";

    YAML::Node config = YAML::LoadFile(configPath);
    if (config["enginePath"] != nullptr)
    {
        std::string path = config["enginePath"].as<std::string>();
        path += "\\Assets\\";
        AssetsPath = path;
    }
    else
    {
        throw "Assets Config not found. Please read the Readme file.";
    }
#else
    GetAssetsPath();
#endif
    m_textureManager.Init();
}

void Shutdown()
{
    CleanAssets();
    m_textureManager.Shutdown();
}

bool CheckIfFileExist(const std::wstring& path)
{
    FILE* file = nullptr;
    if (fopen_s(&file, WstrToStr(path).c_str(), "r") == 0)
    {
        fclose(file);
        return true;
    }
    return false;
}

bool CheckIfFileExist(const std::string& path)
{
    FILE* file = nullptr;
    if (fopen_s(&file, path.c_str(), "r") == 0)
    {
        fclose(file);
        return true;
    }
    return false;
}

void UnloadAsset(const std::string& assetPath)
{
    auto it = m_assets.find(assetPath);
    if (it != m_assets.end())
    {
        SafeDelete(it->second);
        m_assets.erase(assetPath);
    }
}

void CleanAssets()
{
    for (auto& pair : m_assets)
        SafeDelete(pair.second);
    m_assets.clear();
    for (auto& dynAsset : m_dynamicAssets)
        delete dynAsset;
    m_dynamicAssets.clear();
}

void RegisterAsset(Asset* asset)
{
    m_dynamicAssets.push_back(asset);
}

std::string ReadFileAsString(const std::string& path)
{
    std::ifstream f(path);
    std::stringstream buffer;
    buffer << f.rdbuf();
    return buffer.str();
}

template <typename T>
RenderAssetsManager<T>* GetRenderAssetsManager()
{
    throw "Not implemented";
}

template <>
RenderAssetsManager<Renderer::Texture>* GetRenderAssetsManager()
{
    return &m_textureManager;
}

template <>
RenderAssetsManager<Renderer::Shader>* GetRenderAssetsManager()
{
    return &m_shaderManager;
}

template <>
RenderAssetsManager<Renderer::Material>* GetRenderAssetsManager()
{
    return &m_materialManager;
}

bool CheckIfAssetLoaded(const std::string& assetPath)
{
    auto it = m_assets.find(assetPath);
    return it != m_assets.end();
}

template RenderAssetsManager<Renderer::Texture>* GetRenderAssetsManager();
template RenderAssetsManager<Renderer::Shader>* GetRenderAssetsManager();
template RenderAssetsManager<Renderer::Material>* GetRenderAssetsManager();
}
