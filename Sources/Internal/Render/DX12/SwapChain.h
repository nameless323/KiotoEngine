//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Render/DX12/Buffers/ResourceDX12.h"
#include "Render/DX12/StateDX.h"

namespace Kioto::Renderer
{
class SwapChain
{
public:
    SwapChain() = default;
    SwapChain(const SwapChain&) = delete;
    ~SwapChain() = default;

    void Init(const StateDX& state, bool isTearingSupported, uint16 width, uint16 height);

    void Resize(const StateDX& state, uint16 width, uint16 height);

    TextureHandle GetCurrentBackBufferHandle() const;
    TextureHandle GetDepthStencilHandle() const;
    UINT GetCurrentBackBufferIndex() const;
    HRESULT SetFullscreenState(bool fullscreen, IDXGIOutput* output);
    UINT GetCurrentFrameIndex() const;
    DXGI_FORMAT GetBackBufferFormat() const;
    DXGI_FORMAT GetDepthStencilFormat() const;
    ResourceDX12* GetBackBuffer(uint8 index);
    ResourceDX12* GetDepthStencil();
    ResourceDX12* GetCurrentBackBuffer();

    void Present(); // proceed to nex.
    void ProceedToNextFrame();

private:
    const DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    const DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    UINT m_width = -1;
    UINT m_height = -1;
    UINT m_currentFrameIndex = 0;

    bool m_isSwapChainChainInFullScreen = false;
    bool m_isTearingSupported = false;

    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    ResourceDX12 m_backBuffers[StateDX::FrameCount];
    ResourceDX12 m_depthStencil;
};

inline TextureHandle SwapChain::GetCurrentBackBufferHandle() const
{
    return m_backBuffers[m_currentFrameIndex].Handle.GetHandle();
}

inline TextureHandle SwapChain::GetDepthStencilHandle() const
{
    return m_depthStencil.Handle.GetHandle();
}

inline UINT SwapChain::GetCurrentBackBufferIndex() const
{
    return m_swapChain->GetCurrentBackBufferIndex();
}

inline HRESULT SwapChain::SetFullscreenState(bool fullscreen, IDXGIOutput* output)
{
    return m_swapChain->SetFullscreenState(fullscreen, output);
}

inline UINT SwapChain::GetCurrentFrameIndex() const
{
    return m_currentFrameIndex;
}

inline DXGI_FORMAT SwapChain::GetBackBufferFormat() const
{
    return m_backBufferFormat;
}

inline DXGI_FORMAT SwapChain::GetDepthStencilFormat() const
{
    return m_depthStencilFormat;
}

inline ResourceDX12* SwapChain::GetBackBuffer(uint8 index)
{
    return &m_backBuffers[index];
}

inline ResourceDX12* SwapChain::GetDepthStencil()
{
    return &m_depthStencil;
}

inline ResourceDX12* SwapChain::GetCurrentBackBuffer()
{
    return &m_backBuffers[m_swapChain->GetCurrentBackBufferIndex()];
}
}