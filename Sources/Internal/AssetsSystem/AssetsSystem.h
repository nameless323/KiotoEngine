//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>

namespace Kioto::AssetsSystem
{
void Init();
std::wstring GetAssetFullPath(const std::wstring& assetName);
}