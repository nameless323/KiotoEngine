#include "Core/WindowsApplication.h"
#include "Render/DX12/SwapChain.h"

#include <dxgi1_6.h>

#include "DXHelpers.h"
#include "Dx12Helpers/d3dx12.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
using Microsoft::WRL::ComPtr;

void SwapChain::Init(const StateDX& state, bool isTearingSupported, uint16 width, uint16 height)
{
    for (auto& res : mBackBuffers)
        res.SetHandle(GetNewHandle());
    mDepthStencil.SetHandle(GetNewHandle());

    mIsTearingSupported = isTearingSupported;

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = StateDX::FrameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    swapChainDesc.Flags = isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(state.Factory->CreateSwapChainForHwnd(state.CommandQueue.Get(), WindowsApplication::GetHWND(), &swapChainDesc, nullptr, nullptr, &swapChain)); // [a_vorontcov] Fullscreen desc?

    if (isTearingSupported)
        state.Factory->MakeWindowAssociation(WindowsApplication::GetHWND(), DXGI_MWA_NO_ALT_ENTER);
    ThrowIfFailed(swapChain.As(&mSwapChain));

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = StateDX::FrameCount;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    ThrowIfFailed(state.Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&mRtvHeap)));

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    ThrowIfFailed(state.Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap)));
}

void SwapChain::Resize(const StateDX& state, uint16 width, uint16 height)
{
    mCurrentFrameIndex = 0;

    mDepthStencil.Resource.Reset();
    for (auto& swapChainBuffer : mBackBuffers)
        swapChainBuffer.Resource.Reset();

    UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    if (mIsTearingSupported)
        flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    ThrowIfFailed(mSwapChain->ResizeBuffers(StateDX::FrameCount, width, height, mBackBufferFormat, flags));

    BOOL fullscreenState;
    ThrowIfFailed(mSwapChain->GetFullscreenState(&fullscreenState, nullptr));
    mIsSwapChainChainInFullScreen = fullscreenState;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < StateDX::FrameCount; ++i)
    {
        ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mBackBuffers[i].Resource)));
        state.Device->CreateRenderTargetView(mBackBuffers[i].Resource.Get(), nullptr, rtvHandle);
        rtvHandle.Offset(state.RtvDescriptorSize);
    }

    D3D12_RESOURCE_DESC depthStencilDesc = {};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE dsClear = {};
    dsClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsClear.DepthStencil.Depth = 1.0f;
    dsClear.DepthStencil.Stencil = 0;

    auto defaultHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(state.Device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &dsClear, IID_PPV_ARGS(mDepthStencil.Resource.GetAddressOf())));

    D3D12_DEPTH_STENCIL_VIEW_DESC dsViewDesc = {};
    dsViewDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsViewDesc.Texture2D.MipSlice = 0;

    state.Device->CreateDepthStencilView(mDepthStencil.Resource.Get(), &dsViewDesc, mDsvHeap->GetCPUDescriptorHandleForHeapStart());

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencil.Resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    state.CommandList->ResourceBarrier(1, &transition);
}

void SwapChain::Present()
{
    UINT presentFlags = (mIsTearingSupported && !mIsSwapChainChainInFullScreen) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    ThrowIfFailed(mSwapChain->Present(0, presentFlags));
}

void SwapChain::ProceedToNextFrame()
{
    mCurrentFrameIndex = (mCurrentFrameIndex + 1) % StateDX::FrameCount;
}
}