#pragma once

#include "Core/CoreTypes.h"

#include <d3d12.h>
#include <wrl.h>

#include "Render/DX12/Buffers/DefaultHeapBuffer.h"

namespace Kioto::Renderer
{
class DefaultHeapBuffer;

class IndexBufferDX12
{
public:
    IndexBufferDX12(const byte* indexData, uint32 indexDataSize, ID3D12GraphicsCommandList* commandList, ID3D12Device* device, DXGI_FORMAT indexBufferFormat);
    IndexBufferDX12(const IndexBufferDX12&) = delete;
    IndexBufferDX12(IndexBufferDX12&&) = delete;
    IndexBufferDX12& operator=(const IndexBufferDX12&) = delete;
    IndexBufferDX12& operator=(IndexBufferDX12&&) = delete;
    ~IndexBufferDX12();

    ID3D12Resource* GetIndexBuffer() const;
    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const;

private:
    DefaultHeapBuffer* m_buffer = nullptr;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView{};
};

inline const D3D12_INDEX_BUFFER_VIEW& IndexBufferDX12::GetIndexBufferView() const
{
    return m_indexBufferView;
}

inline ID3D12Resource* IndexBufferDX12::GetIndexBuffer() const
{
    return m_buffer->GetBuffer();
}
}