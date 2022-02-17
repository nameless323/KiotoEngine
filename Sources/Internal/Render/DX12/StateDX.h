#pragma once

#include <array>

#include "Core/WindowsCore.h"
#include <dxgi1_6.h>

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
    std::array<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>, FrameCount> CommandAllocators; // [a_vorontcov] For each render thread?
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> CommandList;
};
}
