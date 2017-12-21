//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <d3d12.h>
#include <wrl.h>

namespace Kioto
{
struct TextureDX12
{
    std::wstring Path;
    Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadResource = nullptr;
};
}