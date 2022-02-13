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

    bool IsMemoryAsset() const;

protected:
    bool mIsMemoryAsset = true;
    std::string mAssetPath;
};

inline Asset::Asset()
    : mIsMemoryAsset(true)
{
}

inline Asset::Asset(std::string assetPath) 
    : mAssetPath(std::move(assetPath))
{
    if (!mAssetPath.empty())
        mIsMemoryAsset = false;
}

inline std::string Asset::GetAssetPath() const
{
    return mAssetPath;
}

inline void Asset::SetAssetPath(std::string path)
{
    std::swap(mAssetPath, path);
    if (!mAssetPath.empty())
        mIsMemoryAsset = false;
}

inline bool Asset::IsMemoryAsset() const
{
    return mIsMemoryAsset;
}
}
