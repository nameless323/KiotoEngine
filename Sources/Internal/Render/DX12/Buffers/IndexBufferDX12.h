//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include <d3d12.h>
#include <wrl.h>

namespace Kioto::Renderer
{
class IndexBufferDX12
{
public:
    IndexBufferDX12(byte* vertexData, uint32 vertexDataSize, uint32 vertexStride, ID3D12GraphicsCommandList* commandList, ID3D12Device* device);
    IndexBufferDX12(const IndexBufferDX12&) = delete;
    IndexBufferDX12(IndexBufferDX12&&) = delete;
    IndexBufferDX12& operator=(const IndexBufferDX12&) = delete;
    IndexBufferDX12& operator=(IndexBufferDX12&&) = delete;
    ~IndexBufferDX12() = default;

    ID3D12Resource* GetIndexBuffer() const;
    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer; // [a_vorontsov] TODO:: Check if command list was executed and release ptr. But maybe its not nessesary.
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
};

inline const D3D12_INDEX_BUFFER_VIEW& IndexBufferDX12::GetIndexBufferView() const
{
    return m_indexBufferView;
}

inline ID3D12Resource* IndexBufferDX12::GetIndexBuffer() const
{
    return m_indexBuffer.Get();
}
}