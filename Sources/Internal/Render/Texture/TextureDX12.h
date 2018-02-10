//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <string>
#include <d3d12.h>
#include <wrl.h>

#include "Render/Texture/Texture.h"

namespace Kioto::Renderer
{
class TextureDX12
{
public:
    static DXGI_FORMAT ToDXGIFormat(eTextureFormat format);

    std::wstring Path;
    Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr; // [a_vorontsov] Change to ResourceDX12.
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadResource = nullptr;

    void Create(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

    void SetHandle(TextureHandle handle);
    TextureHandle GetHandle() const;
    DXGI_FORMAT GetFormat() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle() const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;

    void SetCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);
    void SetGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle);

private:
    TextureHandle m_handle;
    D3D12_CPU_DESCRIPTOR_HANDLE m_CPUdescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE m_GPUdescriptorHandle;
};

inline DXGI_FORMAT TextureDX12::ToDXGIFormat(eTextureFormat format)
{
    return static_cast<DXGI_FORMAT>(format);
}

inline void TextureDX12::SetHandle(TextureHandle handle)
{
    m_handle = handle;
}

inline TextureHandle TextureDX12::GetHandle() const
{
    return m_handle;
}

inline DXGI_FORMAT TextureDX12::GetFormat() const
{
    return Resource->GetDesc().Format;
}

inline D3D12_CPU_DESCRIPTOR_HANDLE TextureDX12::GetCPUHandle() const
{
    return m_CPUdescriptorHandle;
}

inline D3D12_GPU_DESCRIPTOR_HANDLE TextureDX12::GetGPUHandle() const
{
    return m_GPUdescriptorHandle;
}

inline void TextureDX12::SetCPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
    m_CPUdescriptorHandle = handle;
}

inline void TextureDX12::SetGPUHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle)
{
    m_GPUdescriptorHandle = handle;
}
}