//
// Copyright (C) Aleksandr Vorontcov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <array>
#include <dxgi.h>
#include <windows.h>

namespace Kioto::Renderer
{
struct StateDX
{
    static constexpr UINT FrameCount = 3;
    UINT CbvSrvUavDescriptorSize = -1;
    UINT RtvDescriptorSize = -1;
    UINT DsvDescriptorSize = -1;
    UINT SamplerDescriptorSize = -1;
    std::array<UINT64, FrameCount> FenceValues;
    UINT64 CurrentFence = 0;

    Microsoft::WRL::ComPtr<IDXGIFactory4> Factory;
    Microsoft::WRL::ComPtr<ID3D12Device> Device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> CommandQueue;

    Microsoft::WRL::ComPtr<ID3D12Fence> Fence;
    std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, StateDX::FrameCount> CommandAllocators; // [a_vorontcov] For each render thread?
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;
};
}
