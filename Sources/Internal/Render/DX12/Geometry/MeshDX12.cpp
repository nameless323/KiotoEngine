#include "stdafx.h"

#include "Render/DX12/Geometry/MeshDX12.h"

#include "Render/DX12/Buffers/IndexBufferDX12.h"
#include "Render/DX12/Buffers/VertexBufferDX12.h"
#include "Render/DX12/StateDX.h"

namespace Kioto::Renderer
{

MeshDX12::~MeshDX12()
{
    SafeDelete(m_vertexBuffer);
    SafeDelete(m_indexBuffer);
}

void MeshDX12::Create(const byte* vertexData, const byte* indexData, uint32 vertexDataSize, uint32 indexDataSize, uint32 vertexDataStride, uint32 vertexCount, uint32 indexCount, const StateDX& state)
{
    SafeDelete(m_vertexBuffer);
    SafeDelete(m_indexBuffer);

    m_vertexCount = vertexCount;
    m_indexCount = indexCount;
    m_vertexStride = vertexDataStride;

    m_vertexBuffer = new VertexBufferDX12(vertexData, vertexDataSize, vertexDataStride, state.CommandList.Get(), state.Device.Get());
    m_indexBuffer = new IndexBufferDX12(indexData, indexDataSize, state.CommandList.Get(), state.Device.Get(), DXGI_FORMAT_R32_UINT);
}

const D3D12_VERTEX_BUFFER_VIEW& MeshDX12::GetVertexBufferView() const
{
    assert(m_vertexBuffer != nullptr);
    return m_vertexBuffer->GetVertexBufferView();
}

const D3D12_INDEX_BUFFER_VIEW& MeshDX12::GetIndexBufferView() const
{
    assert(m_indexBuffer != nullptr);
    return m_indexBuffer->GetIndexBufferView();
}

}