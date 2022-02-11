#include "AssetsSystem/AssetsSystem.h"

#include <yaml-cpp/yaml.h>

#include "AssetsSystem/FilesystemHelpers.h"
#include "Core/CoreHelpers.h"
#include "Render/Material.h"

namespace Kioto::AssetsSystem
{
using std::wstring;
using std::string;

namespace
{
string AssetsPath;

static RenderAssetsManager m_renderAssetsManager;
}

void GetAssetsPath()
{
    static constexpr uint32 pathSize = 512; // [a_vorontcov] So max path to assets can be only 512 chars which is bad. Handle it later.
    WCHAR path[pathSize];
    DWORD size = GetModuleFileName(nullptr, path, pathSize);
    if (size == 0 || size == pathSize)
    {
        throw std::exception(); // [a_vorontcov] Method failed or path was truncated.
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
    if (!FilesystemHelpers::CheckIfFileExist(configPath))
        throw "Assets Config not found. Please read the Readme file.";

    YAML::Node config = YAML::LoadFile(configPath);
    if (config["enginePath"])
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
    m_renderAssetsManager.Init();
}

void Shutdown()
{
    CleanAssets();
    m_renderAssetsManager.Shutdown();
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

RenderAssetsManager* GetRenderAssetsManager()
{
    return &m_renderAssetsManager;
}

bool CheckIfAssetLoaded(const std::string& assetPath)
{
    auto it = m_assets.find(assetPath);
    return it != m_assets.end();
}

}
