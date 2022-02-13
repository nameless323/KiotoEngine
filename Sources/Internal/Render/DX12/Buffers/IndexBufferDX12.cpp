#include "Render/DX12/Buffers/IndexBufferDX12.h"

#include "Core/CoreHelpers.h"
#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
IndexBufferDX12::IndexBufferDX12(const byte* indexData, uint32 indexDataSize, ID3D12GraphicsCommandList* commandList, ID3D12Device* device, DXGI_FORMAT indexBufferFormat)
{
    mBuffer = new DefaultHeapBuffer(indexData, indexDataSize, commandList, device, D3D12_RESOURCE_STATE_INDEX_BUFFER);

    mIndexBufferView.BufferLocation = mBuffer->GetBuffer()->GetGPUVirtualAddress();
    mIndexBufferView.Format = indexBufferFormat;
    mIndexBufferView.SizeInBytes = indexDataSize;
}

IndexBufferDX12::~IndexBufferDX12()
{
    SafeDelete(mBuffer);
}
}