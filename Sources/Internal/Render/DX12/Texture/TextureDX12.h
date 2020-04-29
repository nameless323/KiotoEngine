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
    static DXGI_FORMAT ToDXGIFormat(eResourceFormat format);

    std::wstring Path;
    Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr; // [a_vorontcov] Change to ResourceDX12.
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadResource = nullptr;

    void Create(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

    void SetHandle(TextureHandle handle);
    void SetDescriptor(TextureDescriptor descriptor);
    const TextureDescriptor& GetDescriptor() const;

    TextureHandle GetHandle() const;
    DXGI_FORMAT GetFormat() const;

    bool GetIsFromMemoryAsset() const;
    void SetIsFromMemoryAsset(bool isFromMemoryAsset);

    D3D12_RESOURCE_FLAGS GetDx12TextureFlags() const;

private:
    void CreateFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    void CreateFromDescriptor(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

    TextureHandle m_handle;
    D3D12_CPU_DESCRIPTOR_HANDLE m_CPUdescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE m_GPUdescriptorHandle;
    TextureDescriptor m_descriptor;

    D3D12_RESOURCE_FLAGS m_textureFlags = D3D12_RESOURCE_FLAGS(0);

    bool m_fromMemoryAsset = false;
};

inline DXGI_FORMAT TextureDX12::ToDXGIFormat(eResourceFormat format)
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

inline void TextureDX12::SetDescriptor(TextureDescriptor descriptor)
{
    std::swap(m_descriptor, descriptor);
}

inline bool TextureDX12::GetIsFromMemoryAsset() const
{
    return m_fromMemoryAsset;
}

inline void TextureDX12::SetIsFromMemoryAsset(bool isFromMemoryAsset)
{
    m_fromMemoryAsset = isFromMemoryAsset;
}

inline const TextureDescriptor& TextureDX12::GetDescriptor() const
{
    assert(m_fromMemoryAsset);
    return m_descriptor;
}

inline D3D12_RESOURCE_FLAGS TextureDX12::GetDx12TextureFlags() const
{
    return m_textureFlags;
}
}