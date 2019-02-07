//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

namespace Kioto
{
class Asset
{
public:
    Asset() = delete; // [a_vorontcov] Delete maybe.
    Asset(std::string assetPath); // [a_vorontcov] Separate load maybe.
    Asset(const Asset& other) = default;
    Asset(Asset&& other) = default;
    virtual ~Asset() = default;

    std::string GetAssetPath() const;
    void SetAssetPath(std::string path); // [a_vorontcov] Kostil for mesh parser, remove when separate load appears.

protected:
    std::string m_assetPath;
};

inline Asset::Asset(std::string assetPath) 
    : m_assetPath(assetPath)
{}

inline std::string Asset::GetAssetPath() const
{
    return m_assetPath;
}

inline void Asset::SetAssetPath(std::string path)
{
    std::swap(m_assetPath, path);
}
}
