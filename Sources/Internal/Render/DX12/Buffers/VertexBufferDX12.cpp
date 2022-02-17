#include "Render/DX12/Buffers/VertexBufferDX12.h"

#include "Core/CoreHelpers.h"
#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
VertexBufferDX12::VertexBufferDX12(const byte* vertexData, uint32 vertexDataSize, uint32 vertexStride, ID3D12GraphicsCommandList* commandList, ID3D12Device* device)
{
    mBuffer = new DefaultHeapBuffer(vertexData, vertexDataSize, commandList, device, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);

    mVertexBufferView.BufferLocation = mBuffer->GetBuffer()->GetGPUVirtualAddress();
    mVertexBufferView.StrideInBytes = vertexStride;
    mVertexBufferView.SizeInBytes = vertexDataSize;
}

VertexBufferDX12::~VertexBufferDX12()
{
    SafeDelete(mBuffer);
}
}