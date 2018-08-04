//
// Copyright (C) Alexandr Vorontsov 2018.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Buffers/UploadBufferDX12.h"

namespace Kioto::Renderer
{
UploadBufferDX12::UploadBufferDX12(uint32 framesCount, float32* data, uint32 dataSize, bool isConstantBuffer, ID3D12Device* device)
    : m_isConstantBuffer(isConstantBuffer)
    , m_rawDataSize(dataSize)
    , m_framesCount(framesCount)
{
    if (m_isConstantBuffer)
        m_frameDataSize = GetConstantBufferByteSize(dataSize);
    else
        m_frameDataSize = dataSize;
    m_bufferSize = m_framesCount * m_frameDataSize;

    CD3DX12_HEAP_PROPERTIES hProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize);
    HRESULT hr = device->CreateCommittedResource(
        &hProps,
        D3D12_HEAP_FLAG_NONE,
        &rDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_resource)
    );
    if (!SUCCEEDED(hr))
        return;

    CD3DX12_RANGE readRange(0, 0);
    m_resource->Map(0, &readRange, reinterpret_cast<void**>(&m_data));
}

UploadBufferDX12::~UploadBufferDX12()
{
    if (m_resource != nullptr)
        m_resource->Unmap(0, nullptr);
    m_data = nullptr;
}

ID3D12Resource* UploadBufferDX12::GetResource() const
{
    return m_resource.Get();
}

void UploadBufferDX12::UploadData(uint32 frameIndex, const float32* data)
{
    assert(frameIndex < m_framesCount);
    memcpy(m_data + frameIndex * m_frameDataSize, data, m_rawDataSize);
}

const byte* const UploadBufferDX12::GetBufferStart() const
{
    return m_data;
}

const byte* const UploadBufferDX12::GetBufferEnd() const
{
    return m_data + m_bufferSize;
}

size_t UploadBufferDX12::GetFrameDataSize() const
{
    return m_frameDataSize;
}

size_t UploadBufferDX12::GetBufferSize() const
{
    return m_bufferSize;
}

uint32 UploadBufferDX12::GetFramesCount() const
{
    return m_framesCount;
}

D3D12_GPU_VIRTUAL_ADDRESS UploadBufferDX12::GetFrameDataGpuAddress(uint32 frame) const
{
    return m_resource->GetGPUVirtualAddress() + frame * m_frameDataSize;
}

constexpr uint32 UploadBufferDX12::GetConstantBufferByteSize(uint32 byteSize)
{
    return (byteSize + 255) & ~255;
}
}