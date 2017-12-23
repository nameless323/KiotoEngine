//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <d3d12.h>
#include <wrl.h>

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
struct ResourceDX12
{
    Microsoft::WRL::ComPtr<ID3D12Resource> Resource;
    D3D12_CPU_DESCRIPTOR_HANDLE CPUdescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE GPUdescriptorHandle;
    SafeHandle<ResourceDX12> Handle;
};
}

