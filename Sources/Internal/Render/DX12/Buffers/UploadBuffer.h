//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include <d3d12.h>
#include <wrl.h>
#include "Sources/External/Dx12Helpers/d3dx12.h"

namespace Kioto::Renderer
{
template <typename T>
class UploadBuffer final
{
public:
    UploadBuffer(uint32 elementsCount, bool isConstantBuffer, ID3D12Device* device)
        : m_isConstantBuffer(isConstantBuffer)
        , m_elementsCount(elementsCount)
    {
        if (m_isConstantBuffer)
            m_elemSize = GetConstantBufferByteSize(sizeof(T));
        else
            m_elemSize = sizeof(T);
        m_bufferSize = m_elemSize * elementsCount;

        CD3DX12_HEAP_PROPERTIES hProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_bufferSize);
        HRESULT hr = device->CreateCommittedResource(
            &hProps,
            D3D12_HEAP_FLAG_NONE,
            &rDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_resource)
        );
        if (!SUCCEEDED(hr))
            return;

        CD3DX12_RANGE readRange(0, 0);
        m_resource->Map(0, readRange, reinterpret_cast<void**>(&data));
    }
    UploadBuffer(const UploadBuffer&) = delete;
    UploadBuffer(UploadBuffer&&) = delete;
    UploadBuffer& operator=(const UploadBuffer&) = delete;
    UploadBuffer& operator=(UploadBuffer&&) = delete;
    ~UploadBuffer()
    {
        if (m_resource != nullptr)
            m_resource->Unmap(0, nullptr);
        m_data = nullptr;
    }
    
    ID3D12Resource* GetResource() const
    {
        return m_resource.Get();
    }

    void UploadData(uint32 elementIndex, const T& data)
    {
        memcpy(m_data + elementIndex * m_elemSize, &data, sizeof(T));
    }

    const byte* const GetBuffStart() const
    {
        return m_data;
    }

    const byte* const GetBufferEnd() const
    {
        return m_data + m_bufferSize;
    }

private:
    bool m_isConstantBuffer = false;
    size_t m_elemSize = 0;
    size_t m_bufferSize = 0;
    uint32 m_elementsCount = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
    byte* m_data = nullptr;

    static constexpr uint32 GetConstantBufferByteSize(uint32 byteSize) // [a_vorontsov] Constant buffers must be 255 byte aligned.
    {
        return (byteSize + 255) & ~255;
    }
};
}