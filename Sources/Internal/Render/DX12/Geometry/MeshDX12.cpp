//
// Copyright (C) Alexandr Vorontsov. 2018
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Geometry/MeshDX12.h"

#include "Render/DX12/Buffers/IndexBufferDX12.h"
#include "Render/DX12/Buffers/VertexBufferDX12.h"

namespace Kioto::Renderer
{

MeshDX12::~MeshDX12()
{
    SafeDelete(m_vertexBuffer);
    SafeDelete(m_indexBuffer);
}

void MeshDX12::Create(const byte* VertexData, const byte* indexData, uint32 vertexDataSize, uint32 indexDataSize, uint32 vertexDataStride, uint32 vertexCount, uint32 indexCount)
{

}

}