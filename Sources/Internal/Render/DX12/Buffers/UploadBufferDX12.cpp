#include "Render/DX12/Buffers/UploadBufferDX12.h"

#include <cassert>

#include "Render/DX12/DXHelpers.h"
#include "Render/DX12/StateDX.h"

namespace Kioto::Renderer
{
UploadBufferDX12::UploadBufferDX12(const StateDX& state, byte* data, uint32 elementSize, uint32 elementsCount, bool isConstantBuffer)
    : mIsConstantBuffer(isConstantBuffer)
    , mFramesCount(state.FrameCount)
    , mElementsCount(elementsCount)
{
    mRawDataSize = static_cast<size_t>(elementSize) * static_cast<size_t>(mElementsCount);
    uint32 elementSizeAligned = 0;
    if (mIsConstantBuffer)
    {
        elementSizeAligned = GetConstantBufferByteSize(static_cast<uint32>(elementSize));
        mFrameDataSize = elementSizeAligned * static_cast<size_t>(mElementsCount);
    }
    else
        mFrameDataSize = mRawDataSize;
    mBufferSize = mFramesCount * mFrameDataSize;

    CD3DX12_HEAP_PROPERTIES hProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Buffer(mBufferSize);
    HRESULT hr = state.Device->CreateCommittedResource(
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

    if (mElementsCount > 1)
    {
        mDescriptorSize = state.CbvSrvUavDescriptorSize;
        D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
        heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDescr.NumDescriptors = mElementsCount * mFramesCount;

        ThrowIfFailed(state.Device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&mHeap)));
        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mHeap->GetCPUDescriptorHandleForHeapStart());
        for (size_t i = 0; i < static_cast<size_t>(mElementsCount) * mFramesCount; ++i)
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
            desc.BufferLocation = mResource->GetGPUVirtualAddress() + i * elementSizeAligned;
            desc.SizeInBytes = elementSizeAligned; // [a_vorontcov] Huggge waste of space

            state.Device->CreateConstantBufferView(&desc, handle);
            handle.Offset(state.CbvSrvUavDescriptorSize);
        }
    }
}

UploadBufferDX12::~UploadBufferDX12()
{
    if (mResource != nullptr)
        mResource->Unmap(0, nullptr);
    mData = nullptr;
}

ID3D12Resource* UploadBufferDX12::GetResource() const
{
    return mResource.Get();
}

void UploadBufferDX12::UploadData(uint32 frameIndex, const byte* data)
{
    assert(frameIndex < mFramesCount);
    memcpy(mData + frameIndex * mFrameDataSize, data, mRawDataSize);
}

const byte* const UploadBufferDX12::GetBufferStart() const
{
    return mData;
}

const byte* const UploadBufferDX12::GetBufferEnd() const
{
    return mData + mBufferSize;
}

size_t UploadBufferDX12::GetFrameDataSize() const
{
    return mFrameDataSize;
}

size_t UploadBufferDX12::GetBufferSize() const
{
    return mBufferSize;
}

uint32 UploadBufferDX12::GetFramesCount() const
{
    return mFramesCount;
}

D3D12_GPU_VIRTUAL_ADDRESS UploadBufferDX12::GetFrameDataGpuAddress(uint32 frame) const
{
    return mResource->GetGPUVirtualAddress() + frame * mFrameDataSize;
}

constexpr uint32 UploadBufferDX12::GetConstantBufferByteSize(uint32 byteSize)
{
    return (byteSize + 255) & ~255;
}

}