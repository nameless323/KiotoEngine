//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include <string>

#pragma once

namespace Kioto
{
class Asset
{
public:
    Asset() = default; // [a_vorontsov] Delete maybe.
    Asset(std::string assetPath);
    Asset(const Asset& other) = default;
    Asset(Asset&& other) = default;
    virtual ~Asset() = default;

    std::string GetAssetPath() const;

protected:
    std::string m_assetPath;
};

inline Asset::Asset(std::string assetPath) : m_assetPath(assetPath)
{}

inline std::string Asset::GetAssetPath() const
{
    return m_assetPath;
}
}
