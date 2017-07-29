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
    void Shutdown();

private:
    void GetHardwareAdapter(IDXGIFactory4* factory, IDXGIAdapter1** adapter);
    void WaitForGPU();

    static constexpr UINT FrameCount = 3;

    bool m_isTearingSupported = false;
    UINT m_frameIndex = -1;
    UINT m_cbvSrvUavDescriptorSize = -1;
    UINT m_rtvDescriptorSize = -1;
    UINT m_dsvDescriptorSize = -1;
    UINT m_samplerDescriptorSize = -1;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    UINT64 m_currFenceValue = 0;
};

class ComException : public std::exception // [a_vorontsov] https://github.com/Microsoft/DirectXTK/wiki/ThrowIfFailed
{
public:
    ComException(HRESULT hr) 
        : m_result(hr) 
    {}

    virtual const char* what() const override
    {
        static char str[64] = { 0 };
        sprintf_s(str, "Failure with HRESULT of %08X", m_result);
        return str;
    }

private:
    HRESULT m_result = -1;
};

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw ComException(hr);
    }
}

#if defined(_DEBUG)
inline void SetName(ID3D12Object* object, LPCWSTR name)
{
    object->SetName(name);
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
#endif

#define NAME_D3D12_OBJECT(x) SetName(x.Get(), L#x)

}