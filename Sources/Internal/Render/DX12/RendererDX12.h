//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <cstdio>
#include <exception>

namespace Kioto::Renderer
{

class RendererDX12 final
{
public:
    RendererDX12() = default;
    RendererDX12(const RendererDX12&) = delete;
    RendererDX12(RendererDX12&&) = delete;
    RendererDX12& operator= (const RendererDX12&) = delete;
    RendererDX12& operator= (const RendererDX12&&) = delete;
    ~RendererDX12() = default;

    void Init(uint16 width, uint16 height);
    void Resize(uint16 width, uint16 heigth);
    void ChangeFullScreenMode(bool fullScreen);
    void Shutdown();
    void Present();

    ID3D12Resource* GetCurrentBackBuffer() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

private:
    void GetHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter1** adapter);
    void WaitForGPU();
    void LogAdapters();
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

    static constexpr UINT FrameCount = 3;

    bool m_isTearingSupported = false; // [a_vorontsov] TODO: Properly handle when tearing is not supported.
    UINT m_frameIndex = -1;
    UINT m_cbvSrvUavDescriptorSize = -1;
    UINT m_rtvDescriptorSize = -1;
    UINT m_dsvDescriptorSize = -1;
    UINT m_samplerDescriptorSize = -1;
    UINT64 m_currFenceValue = 0;
    DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT m_width = -1;
    UINT m_height = -1;
    bool m_isFullScreen = false;
    bool m_isSwapChainChainInFullScreen = false;

    Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffers[FrameCount];
    Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator; // [a_vorontsov] For each render thread?
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

    D3D12_VIEWPORT m_viewport = {};
    D3D12_RECT m_scissor = {};
};

}