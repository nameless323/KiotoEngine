#include "Render/DX12/Geometry/MeshDX12.h"

#include <cassert>

#include "Core/CoreHelpers.h"
#include "Render/DX12/Buffers/IndexBufferDX12.h"
#include "Render/DX12/Buffers/VertexBufferDX12.h"
#include "Render/DX12/StateDX.h"

namespace Kioto::Renderer
{

MeshDX12::~MeshDX12()
{
    SafeDelete(mVertexBuffer);
    SafeDelete(mIndexBuffer);
}

void MeshDX12::Create(const byte* vertexData, const byte* indexData, uint32 vertexDataSize, uint32 indexDataSize, uint32 vertexDataStride, uint32 vertexCount, uint32 indexCount, const StateDX& state)
{
    SafeDelete(mVertexBuffer);
    SafeDelete(mIndexBuffer);

    mVertexCount = vertexCount;
    mIndexCount = indexCount;
    mVertexStride = vertexDataStride;

    mVertexBuffer = new VertexBufferDX12(vertexData, vertexDataSize, vertexDataStride, state.CommandList.Get(), state.Device.Get());
    mIndexBuffer = new IndexBufferDX12(indexData, indexDataSize, state.CommandList.Get(), state.Device.Get(), DXGI_FORMAT_R32_UINT);
}

const D3D12_VERTEX_BUFFER_VIEW& MeshDX12::GetVertexBufferView() const
{
    assert(mVertexBuffer != nullptr);
    return mVertexBuffer->GetVertexBufferView();
}

const D3D12_INDEX_BUFFER_VIEW& MeshDX12::GetIndexBufferView() const
{
    assert(mIndexBuffer != nullptr);
    return mIndexBuffer->GetIndexBufferView();
}

}