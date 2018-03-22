//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class VertexBufferDX12;
class IndexBufferDX12;

class MeshDX12
{
public:
    MeshDX12() = default;
    ~MeshDX12();

    MeshHandle GetHandle() const;
    void SetHandle(MeshHandle handle);

    void Create(const byte* VertexData, const byte* indexData, uint32 vertexDataSize, uint32 indexDataSize, uint32 vertexDataStride, uint32 vertexCount, uint32 indexCount);

private:
    VertexBufferDX12* m_vertexBuffer = nullptr;
    IndexBufferDX12* m_indexBuffer = nullptr;
    uint32 m_vertexCount = 0;
    uint32 m_indexCount = 0;

    MeshHandle m_handle;
};

inline MeshHandle MeshDX12::GetHandle() const
{
    return m_handle;
}

inline void MeshDX12::SetHandle(MeshHandle handle)
{
    m_handle = handle;
}

}