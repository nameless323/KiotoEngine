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

    D3D12_RESOURCE_STATES GetCurrentState() const;
    void SetCurrentState(D3D12_RESOURCE_STATES state);

    const std::string& GetDebugName() const;

private:
    void CreateFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    void CreateFromDescriptor(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

    TextureHandle mHandle;
    D3D12_CPU_DESCRIPTOR_HANDLE mCPUdescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE mGPUdescriptorHandle;
    TextureDescriptor mDescriptor;

    D3D12_RESOURCE_STATES mCurrentState;

    D3D12_RESOURCE_FLAGS mTextureFlags = D3D12_RESOURCE_FLAGS(0);

    bool mFromMemoryAsset = false;

#ifdef _DEBUG
    bool mIsDescriptorInitialized = false;
#endif
};

inline DXGI_FORMAT TextureDX12::ToDXGIFormat(eResourceFormat format)
{
    return static_cast<DXGI_FORMAT>(format);
}

inline void TextureDX12::SetHandle(TextureHandle handle)
{
    mHandle = handle;
}

inline TextureHandle TextureDX12::GetHandle() const
{
    return mHandle;
}

inline DXGI_FORMAT TextureDX12::GetFormat() const
{
    return Resource->GetDesc().Format;
}

inline void TextureDX12::SetDescriptor(TextureDescriptor descriptor)
{
#ifdef _DEBUG
    mIsDescriptorInitialized = true;
#endif
    std::swap(mDescriptor, descriptor);
}

inline bool TextureDX12::GetIsFromMemoryAsset() const
{
    return mFromMemoryAsset;
}

inline void TextureDX12::SetIsFromMemoryAsset(bool isFromMemoryAsset)
{
    mFromMemoryAsset = isFromMemoryAsset;
}

inline const TextureDescriptor& TextureDX12::GetDescriptor() const
{
    assert(mFromMemoryAsset);
    return mDescriptor;
}

inline D3D12_RESOURCE_FLAGS TextureDX12::GetDx12TextureFlags() const
{
    return mTextureFlags;
}

inline D3D12_RESOURCE_STATES TextureDX12::GetCurrentState() const
{
    return mCurrentState;
}

inline void TextureDX12::SetCurrentState(D3D12_RESOURCE_STATES state)
{
    mCurrentState = state;
}

inline const std::string& TextureDX12::GetDebugName() const
{
#ifdef _DEBUG
    if (!mIsDescriptorInitialized)
        assert("Initialize texture descriptor before query the texture for the name" && false);
#endif
    return mDescriptor.Name;
}

}