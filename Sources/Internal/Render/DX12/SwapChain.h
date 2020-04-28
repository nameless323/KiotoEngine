#pragma once

#include "Render/DX12/Buffers/ResourceDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/DX12/Texture/TextureDX12.h"

namespace Kioto::Renderer
{
class SwapChain
{
public:
    SwapChain() = default;
    SwapChain(const SwapChain&) = delete;
    ~SwapChain() = default;

    SwapChain& operator= (const SwapChain&) = delete;

    void Init(const StateDX& state, bool isTearingSupported, uint16 width, uint16 height);

    void Resize(const StateDX& state, uint16 width, uint16 height);

    TextureHandle GetCurrentBackBufferHandle() const;
    TextureHandle GetDepthStencilHandle() const;
    UINT GetCurrentBackBufferIndex() const;
    HRESULT SetFullscreenState(bool fullscreen, IDXGIOutput* output);
    UINT GetCurrentFrameIndex() const;
    DXGI_FORMAT GetBackBufferFormat() const;
    DXGI_FORMAT GetDepthStencilFormat() const;
    TextureDX12* GetBackBuffer(uint8 index);
    TextureDX12* GetDepthStencil();
    TextureDX12* GetCurrentBackBuffer();

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorHandleForHeapStart() const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandleForHeapStart() const;

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
    TextureDX12 m_backBuffers[StateDX::FrameCount];
    TextureDX12 m_depthStencil;
};

inline TextureHandle SwapChain::GetCurrentBackBufferHandle() const
{
    return m_backBuffers[m_currentFrameIndex].GetHandle();
}

inline TextureHandle SwapChain::GetDepthStencilHandle() const
{
    return m_depthStencil.GetHandle();
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

inline TextureDX12* SwapChain::GetBackBuffer(uint8 index)
{
    return &m_backBuffers[index];
}

inline TextureDX12* SwapChain::GetDepthStencil()
{
    return &m_depthStencil;
}

inline TextureDX12* SwapChain::GetCurrentBackBuffer()
{
    return &m_backBuffers[m_swapChain->GetCurrentBackBufferIndex()];
}

inline D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCpuDescriptorHandleForHeapStart() const
{
    return m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
}

inline D3D12_GPU_DESCRIPTOR_HANDLE SwapChain::GetGpuDescriptorHandleForHeapStart() const
{
    return m_rtvHeap->GetGPUDescriptorHandleForHeapStart();
}
}