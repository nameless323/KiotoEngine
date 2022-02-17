#pragma once

#include <d3d12.h>

#include "Core/CoreTypes.h"

#include "Render/DX12/Buffers/DefaultHeapBuffer.h"

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
    ~VertexBufferDX12();

    ID3D12Resource* GetVertexBuffer() const;
    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const;

private:
    DefaultHeapBuffer* mBuffer = nullptr;
    D3D12_VERTEX_BUFFER_VIEW mVertexBufferView{};
};

inline const D3D12_VERTEX_BUFFER_VIEW& VertexBufferDX12::GetVertexBufferView() const
{
    return mVertexBufferView;
}

inline ID3D12Resource* VertexBufferDX12::GetVertexBuffer() const
{
    return mBuffer->GetBuffer();
}
}