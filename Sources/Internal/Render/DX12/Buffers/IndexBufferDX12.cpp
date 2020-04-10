#include "stdafx.h"

#include "Render/DX12/Buffers/IndexBufferDX12.h"

#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
IndexBufferDX12::IndexBufferDX12(const byte* indexData, uint32 indexDataSize, ID3D12GraphicsCommandList* commandList, ID3D12Device* device, DXGI_FORMAT indexBufferFormat)
{
    m_buffer = new DefaultHeapBuffer(indexData, indexDataSize, commandList, device, D3D12_RESOURCE_STATE_INDEX_BUFFER);

    m_indexBufferView.BufferLocation = m_buffer->GetBuffer()->GetGPUVirtualAddress();
    m_indexBufferView.Format = indexBufferFormat;
    m_indexBufferView.SizeInBytes = indexDataSize;
}

IndexBufferDX12::~IndexBufferDX12()
{
    SafeDelete(m_buffer);
}
}