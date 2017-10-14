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
class VertexBufferDX12
{
public:
    VertexBufferDX12(const byte* vertexData, uint32 vertexDataSize, uint32 vertexStride, ID3D12GraphicsCommandList* commandList, ID3D12Device* device);
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

inline const D3D12_VERTEX_BUFFER_VIEW& VertexBufferDX12::GetVertexBufferView() const
{
    return m_vertexBufferView;
}

inline ID3D12Resource* VertexBufferDX12::GetVertexBuffer() const
{
    return m_vertexBuffer.Get();
}
}