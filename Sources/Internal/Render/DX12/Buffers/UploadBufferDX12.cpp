#include "stdafx.h"

#include "Render/DX12/Buffers/UploadBufferDX12.h"
#include "Render/DX12/StateDX.h"

namespace Kioto::Renderer
{
UploadBufferDX12::UploadBufferDX12(const StateDX& state, byte* data, uint32 elementSize, uint32 elementsCount, bool isConstantBuffer)
    : m_isConstantBuffer(isConstantBuffer)
    , m_framesCount(state.FrameCount)
    , m_elementsCount(elementsCount)
{
    m_rawDataSize = static_cast<size_t>(elementSize) * static_cast<size_t>(m_elementsCount);
    uint32 elementSizeAligned = 0;
    if (m_isConstantBuffer)
    {
        elementSizeAligned = GetConstantBufferByteSize(static_cast<uint32>(elementSize));
        m_frameDataSize = elementSizeAligned * static_cast<size_t>(m_elementsCount);
    }
    else
        m_frameDataSize = m_rawDataSize;
    m_bufferSize = m_framesCount * m_frameDataSize;

    CD3DX12_HEAP_PROPERTIES hProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize);
    HRESULT hr = state.Device->CreateCommittedResource(
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

    if (m_elementsCount > 1)
    {
        m_descriptorSize = state.CbvSrvUavDescriptorSize;
        D3D12_DESCRIPTOR_HEAP_DESC heapDescr = {};
        heapDescr.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        heapDescr.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDescr.NumDescriptors = m_elementsCount * m_framesCount;

        ThrowIfFailed(state.Device->CreateDescriptorHeap(&heapDescr, IID_PPV_ARGS(&m_heap)));
        CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_heap->GetCPUDescriptorHandleForHeapStart());
        for (size_t i = 0; i < static_cast<size_t>(m_elementsCount) * m_framesCount; ++i)
        {
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
            desc.BufferLocation = m_resource->GetGPUVirtualAddress() + i * elementSizeAligned;
            desc.SizeInBytes = elementSizeAligned; // [a_vorontcov] Huggge waste of space

            state.Device->CreateConstantBufferView(&desc, handle);
            handle.Offset(state.CbvSrvUavDescriptorSize);
        }
    }
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

void UploadBufferDX12::UploadData(uint32 frameIndex, const byte* data)
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