#pragma once

#include "Render/DX12/Buffers/ResourceDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/DX12/Texture/TextureDX12.h"
#include "Dx12Helpers/d3dx12.h"

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

    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferCPUHandle(const StateDX& state) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilCPUHandle() const;

    void Present(); // proceed to nex.
    void ProceedToNextFrame();

private:
    const DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    UINT mWidth = -1;
    UINT mHeight = -1;
    UINT mCurrentFrameIndex = 0;

    bool mIsSwapChainChainInFullScreen = false;
    bool mIsTearingSupported = false;

    Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;
    TextureDX12 mBackBuffers[StateDX::FrameCount];
    TextureDX12 mDepthStencil;
};

inline TextureHandle SwapChain::GetCurrentBackBufferHandle() const
{
    return mBackBuffers[mCurrentFrameIndex].GetHandle();
}

inline TextureHandle SwapChain::GetDepthStencilHandle() const
{
    return mDepthStencil.GetHandle();
}

inline UINT SwapChain::GetCurrentBackBufferIndex() const
{
    return mSwapChain->GetCurrentBackBufferIndex();
}

inline HRESULT SwapChain::SetFullscreenState(bool fullscreen, IDXGIOutput* output)
{
    return mSwapChain->SetFullscreenState(fullscreen, output);
}

inline UINT SwapChain::GetCurrentFrameIndex() const
{
    return mCurrentFrameIndex;
}

inline DXGI_FORMAT SwapChain::GetBackBufferFormat() const
{
    return mBackBufferFormat;
}

inline DXGI_FORMAT SwapChain::GetDepthStencilFormat() const
{
    return mDepthStencilFormat;
}

inline TextureDX12* SwapChain::GetBackBuffer(uint8 index)
{
    return &mBackBuffers[index];
}

inline TextureDX12* SwapChain::GetDepthStencil()
{
    return &mDepthStencil;
}

inline TextureDX12* SwapChain::GetCurrentBackBuffer()
{
    return &mBackBuffers[mSwapChain->GetCurrentBackBufferIndex()];
}

inline D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCpuDescriptorHandleForHeapStart() const
{
    return mRtvHeap->GetCPUDescriptorHandleForHeapStart();
}

inline D3D12_GPU_DESCRIPTOR_HANDLE SwapChain::GetGpuDescriptorHandleForHeapStart() const
{
    return mRtvHeap->GetGPUDescriptorHandleForHeapStart();
}

inline D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetDepthStencilCPUHandle() const
{
    return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

inline D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCurrentBackBufferCPUHandle(const StateDX& state) const
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    handle.Offset(GetCurrentBackBufferIndex() * state.RtvDescriptorSize);
    return handle;
}
}