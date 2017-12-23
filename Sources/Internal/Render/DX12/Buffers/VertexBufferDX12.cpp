//
// Copyright (C) Alexandr Vorontsov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Buffers/VertexBufferDX12.h"

#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
VertexBufferDX12::VertexBufferDX12(const byte* vertexData, uint32 vertexDataSize, uint32 vertexStride, ID3D12GraphicsCommandList* commandList, ID3D12Device* device)
{
    m_buffer = new DefaultHeapBuffer(vertexData, vertexDataSize, commandList, device, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    m_vertexBufferView.BufferLocation = m_buffer->GetBuffer()->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = vertexStride;
    m_vertexBufferView.SizeInBytes = vertexDataSize;
}

VertexBufferDX12::~VertexBufferDX12()
{
    SafeDelete(m_buffer);
}

}