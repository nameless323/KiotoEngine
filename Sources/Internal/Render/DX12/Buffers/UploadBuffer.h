//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include <d3d12.h>
#include <wrl.h>

namespace Kioto::Renderer
{
template <typename T>
class UploadBuffer
{
public:
    UploadBuffer(uint32 elementsCount, bool isConstantBuffer, ID3D12Device* device)
        : m_isConstantBuffer(isConstantBuffer)
    {

    }

private:
    bool m_isConstantBuffer = false;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
};
}