#pragma once

#include <string>

namespace Kioto
{
class Asset
{
public:
    Asset(); // [a_vorontcov] For memory assets.
    Asset(std::string assetPath); // [a_vorontcov] Separate load maybe.
    Asset(const Asset& other) = default;
    Asset(Asset&& other) = default;
    virtual ~Asset() = default;

    std::string GetAssetPath() const;
    void SetAssetPath(std::string path); // [a_vorontcov] Kostil for mesh parser, remove when separate load appears.

protected:
    bool m_isMemoryAsset = true;
    std::string m_assetPath;
};

inline Asset::Asset(std::string assetPath) 
    : m_assetPath(assetPath)
{
    if (!m_assetPath.empty())
        m_isMemoryAsset = false;
}

inline std::string Asset::GetAssetPath() const
{
    return m_assetPath;
}

inline void Asset::SetAssetPath(std::string path)
{
    std::swap(m_assetPath, path);
    if (!m_assetPath.empty())
        m_isMemoryAsset = false;
}
}
