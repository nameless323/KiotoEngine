#pragma once

#include "Core/CoreTypes.h"

#include <d3d12.h>
#include <wrl.h>
#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
template <typename T>
class UploadBuffer final
{
public:
    UploadBuffer(uint32 elementsCount, bool isConstantBuffer, ID3D12Device* device);
    UploadBuffer(const UploadBuffer&) = delete;
    UploadBuffer(UploadBuffer&&) = delete;
    UploadBuffer& operator=(const UploadBuffer&) = delete;
    UploadBuffer& operator=(UploadBuffer&&) = delete;
    ~UploadBuffer();

    ID3D12Resource* GetResource() const;
    void UploadData(uint32 elementIndex, const T& data);
    void UploadData(const T* data);
    const byte* const GetBufferStart() const;
    const byte* const GetBufferEnd() const;
    size_t GetElementSize() const;
    size_t GetBufferSize() const;
    uint32 GetElementsCount() const;

    D3D12_GPU_VIRTUAL_ADDRESS GetElementGpuAddress(uint32 elem) const;

private:
    bool mIsConstantBuffer = false;
    size_t mElemSize = 0;
    size_t mBufferSize = 0;
    uint32 mElementsCount = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource;
    byte* mData = nullptr;

    static constexpr uint32 GetConstantBufferByteSize(uint32 byteSize); // [a_vorontcov] Constant buffers must be 255 byte aligned.
};

template <typename T>
UploadBuffer<T>::UploadBuffer(uint32 elementsCount, bool isConstantBuffer, ID3D12Device* device)
    : mIsConstantBuffer(isConstantBuffer)
    , mElementsCount(elementsCount)
{
    if (mIsConstantBuffer)
        mElemSize = GetConstantBufferByteSize(sizeof(T));
    else
        mElemSize = sizeof(T);
    mBufferSize = mElemSize * elementsCount;

    CD3DX12_HEAP_PROPERTIES hProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Buffer(mBufferSize);
    HRESULT hr = device->CreateCommittedResource(
        &hProps,
        D3D12_HEAP_FLAG_NONE,
        &rDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&mResource)
    );
    if (!SUCCEEDED(hr))
        return;

    CD3DX12_RANGE readRange(0, 0);
    mResource->Map(0, &readRange, reinterpret_cast<void**>(&mData));
}

template <typename T>
UploadBuffer<T>::~UploadBuffer()
{
    if (mResource != nullptr)
        mResource->Unmap(0, nullptr);
    mData = nullptr;
}

template <typename T>
ID3D12Resource* UploadBuffer<T>::GetResource() const
{
    return mResource.Get();
}

template <typename T>
void UploadBuffer<T>::UploadData(uint32 elementIndex, const T& data)
{
    memcpy(mData + elementIndex * mElemSize, &data, sizeof(T));
}

template <typename T>
void UploadBuffer<T>::UploadData(const T* data)
{
    memcpy(mData, data, mBufferSize);
}

template <typename T>
const byte* const UploadBuffer<T>::GetBufferStart() const
{
    return mData;
}

template <typename T>
const byte* const UploadBuffer<T>::GetBufferEnd() const 
{
    return mData + mBufferSize;
}

template <typename T>
size_t UploadBuffer<T>::GetElementSize() const
{
    return mElemSize;
}

template <typename T>
size_t UploadBuffer<T>::GetBufferSize() const
{
    return mBufferSize;
}

template <typename T>
uint32 UploadBuffer<T>::GetElementsCount() const
{
    return mElementsCount;
}

template <typename T>
D3D12_GPU_VIRTUAL_ADDRESS UploadBuffer<T>::GetElementGpuAddress(uint32 elem) const
{
    return mResource->GetGPUVirtualAddress() + elem * mElemSize;
}

template <typename T>
constexpr uint32 UploadBuffer<T>::GetConstantBufferByteSize(uint32 byteSize)
{
    return (byteSize + 255) & ~255;
}
}