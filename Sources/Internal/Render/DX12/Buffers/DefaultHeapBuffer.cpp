#include "Render/DX12/Buffers/DefaultHeapBuffer.h"

#include "Render/DX12/DXHelpers.h"
#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
DefaultHeapBuffer::DefaultHeapBuffer(const byte* data, uint32 dataSize, ID3D12GraphicsCommandList* commandList, ID3D12Device* device, D3D12_RESOURCE_STATES destinationState)
{
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_uploadBuffer)));

    UINT8* mappedBuffer = nullptr;
    CD3DX12_RANGE range(0, 0);
    m_uploadBuffer->Map(0, &range, reinterpret_cast<void**>(&mappedBuffer));
    memcpy(mappedBuffer, data, dataSize);
    m_uploadBuffer->Unmap(0, nullptr);

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_buffer)));

    commandList->CopyResource(m_buffer.Get(), m_uploadBuffer.Get());
    CD3DX12_RESOURCE_BARRIER toDest = CD3DX12_RESOURCE_BARRIER::Transition(m_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, destinationState);
    commandList->ResourceBarrier(1, &toDest);
}
}