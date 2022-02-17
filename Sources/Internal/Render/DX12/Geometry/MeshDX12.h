#pragma once

#include <d3d12.h>

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class VertexBufferDX12;
class IndexBufferDX12;
struct StateDX;

class MeshDX12
{
public:
    MeshDX12() = default;
    ~MeshDX12();

    MeshHandle GetHandle() const;
    void SetHandle(MeshHandle handle);

    void Create(const byte* vertexData, const byte* indexData, uint32 vertexDataSize, uint32 indexDataSize, uint32 vertexDataStride, uint32 vertexCount, uint32 indexCount, const StateDX& state);

    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const;
    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const;

    uint32 GetIndexCount() const;

private:
    VertexBufferDX12* mVertexBuffer = nullptr;
    IndexBufferDX12* mIndexBuffer = nullptr;
    uint32 mVertexCount = 0;
    uint32 mVertexStride = 0;
    uint32 mIndexCount = 0;

    MeshHandle mHandle;
};

inline MeshHandle MeshDX12::GetHandle() const
{
    return mHandle;
}

inline void MeshDX12::SetHandle(MeshHandle handle)
{
    mHandle = handle;
}

inline uint32 MeshDX12::GetIndexCount() const
{
    return mIndexCount;
}
}