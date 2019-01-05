//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include <d3d12.h>
#include <wrl.h>

namespace Kioto::Renderer
{
class DefaultHeapBuffer
{
public:
    DefaultHeapBuffer(const byte* data, uint32 dataSize, ID3D12GraphicsCommandList* commandList, ID3D12Device* device, D3D12_RESOURCE_STATES destinationState);
    DefaultHeapBuffer(const DefaultHeapBuffer&) = delete;
    DefaultHeapBuffer(DefaultHeapBuffer&&) = delete;
    DefaultHeapBuffer& operator=(const DefaultHeapBuffer&) = delete;
    DefaultHeapBuffer& operator=(DefaultHeapBuffer&&) = delete;
    ~DefaultHeapBuffer() = default;

    ID3D12Resource* GetBuffer() const;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> m_buffer;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer; // [a_vorontcov] TODO:: Check if command list was executed and release ptr. But maybe its not nessesary.
};

inline ID3D12Resource* DefaultHeapBuffer::GetBuffer() const
{
    return m_buffer.Get();
}
}