//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include <d3d12.h>
#include <wrl.h>
#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
template <typename T>
class VertexBufferDX12
{
public:
    VertexBufferDX12(T* vertexData, uint32 vertexDataSize, uint32 vertexStride, ID3D12GraphicsCommandList* commandList, ID3D12Device* device);
    VertexBufferDX12(const VertexBufferDX12&) = delete;
    VertexBufferDX12(VertexBufferDX12&&) = delete;
    VertexBufferDX12& operator=(const VertexBufferDX12&) = delete;
    VertexBufferDX12& operator=(VertexBufferDX12&&) = delete;
    ~VertexBufferDX12() = default;

    ID3D12Resource* GetVertexBuffer() const;
    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer; // [a_vorontsov] TODO:: Check if command list was executed and release ptr. But maybe its not nessesary.
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView{};
};

template <typename T>
VertexBufferDX12<T>::VertexBufferDX12(T* vertexData, uint32 vertexDataSize, uint32 vertexStride, ID3D12GraphicsCommandList* commandList, ID3D12Device* device)
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

template <typename T>
const D3D12_VERTEX_BUFFER_VIEW& VertexBufferDX12<T>::GetVertexBufferView() const
{
    return m_vertexBufferView;
}

template <typename T>
ID3D12Resource* VertexBufferDX12<T>::GetVertexBuffer() const
{
    return m_vertexBuffer;
}
}