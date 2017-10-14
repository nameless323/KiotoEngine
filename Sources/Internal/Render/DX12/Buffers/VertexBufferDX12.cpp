//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Buffers/VertexBufferDX12.h"

namespace Kioto::Renderer
{
VertexBufferDX12::VertexBufferDX12(byte* vertexData, uint32 vertexDataSize, uint32 vertexStride, ID3D12GraphicsCommandList* commandList, ID3D12Device* device)
{
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC vertBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexDataSize);
    ThrowIfFailed(device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &vertBufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_uploadBuffer)));

    UINT8* mappedBuffer = nullptr;
    CD3DX12_RANGE range(0, 0);
    m_uploadBuffer->Map(0, &range, reinterpret_cast<void**>(&mappedBuffer));
    memcpy(mappedBuffer, vertexData, vertexDataSize);
    m_uploadBuffer->Unmap(0, nullptr);

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &vertBufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)));

    commandList->CopyResource(m_vertexBuffer.Get(), m_uploadBuffer.Get());
    CD3DX12_RESOURCE_BARRIER toVertBuffer = CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    commandList->ResourceBarrier(1, &toVertBuffer);

    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = vertexStride;
    m_vertexBufferView.SizeInBytes = vertexDataSize;
}
}