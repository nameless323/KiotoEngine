#pragma once

#include "Core/CoreTypes.h"

#include <d3d12.h>
#include <wrl.h>
#include <variant>
#include "Sources/External/Dx12Helpers/d3dx12.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
struct StateDX;

class UploadBufferDX12 final
{
public:
    UploadBufferDX12(const StateDX& state, byte* data, uint32 elementSize, uint32 elementsCount, bool isConstantBuffer); // todo: [a_vorontcov] data is unused
    UploadBufferDX12(const UploadBufferDX12&) = delete;
    UploadBufferDX12(UploadBufferDX12&&) = delete;
    UploadBufferDX12& operator=(const UploadBufferDX12&) = delete;
    UploadBufferDX12& operator=(UploadBufferDX12&&) = delete;
    ~UploadBufferDX12();

    ID3D12Resource* GetResource() const;
    void UploadData(uint32 frameIndex, const byte* data);
    const byte* const GetBufferStart() const;
    const byte* const GetBufferEnd() const;
    size_t GetFrameDataSize() const;
    size_t GetBufferSize() const;
    uint32 GetFramesCount() const;
    ID3D12DescriptorHeap* GetDescriptorHeap() const;
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandleForFrame(uint32 frame) const;

    D3D12_GPU_VIRTUAL_ADDRESS GetFrameDataGpuAddress(uint32 frame) const;

    void IncrementUpdatedFramesCount();
    void DecrementUpdatedFramesCount();
    void ResetUpdatedFramesCount();
    uint32 GetUpdatedFramesCount() const;
    bool IsUpdated() const;
    bool HasDescriptorHeap() const;

    template <typename T>
    T GetHandle() const;

private:
    bool m_isConstantBuffer = false;
    size_t m_frameDataSize = 0;
    size_t m_rawDataSize = 0;
    size_t m_bufferSize = 0;
    uint32 m_framesCount = 0;
    uint32 m_framesUpdated = 0;
    uint32 m_elementsCount = 1;
    uint32 m_descriptorSize = 0;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
    byte* m_data = nullptr;
    std::variant<ConstantBufferHandle> m_handle;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_heap;

    static constexpr uint32 GetConstantBufferByteSize(uint32 byteSize); // [a_vorontcov] Constant buffers must be 255 byte aligned.
};

inline void UploadBufferDX12::IncrementUpdatedFramesCount()
{
    ++m_framesUpdated;
}

inline void UploadBufferDX12::DecrementUpdatedFramesCount()
{
    --m_framesUpdated;
    m_framesUpdated = m_framesUpdated < 0 ? 0 : m_framesUpdated;
}

inline void UploadBufferDX12::ResetUpdatedFramesCount()
{
    m_framesUpdated = 0;
}

inline uint32 UploadBufferDX12::GetUpdatedFramesCount() const
{
    return m_framesUpdated;
}

inline bool UploadBufferDX12::IsUpdated() const
{
    return m_framesUpdated >= m_framesCount;
}

inline bool UploadBufferDX12::HasDescriptorHeap() const
{
    return m_elementsCount > 1;
}

inline ID3D12DescriptorHeap* UploadBufferDX12::GetDescriptorHeap() const
{
    assert(HasDescriptorHeap());
    return m_heap.Get();
}


inline D3D12_GPU_DESCRIPTOR_HANDLE UploadBufferDX12::GetGpuDescriptorHandleForFrame(uint32 frame) const
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_heap->GetGPUDescriptorHandleForHeapStart());
    handle.Offset(frame * m_elementsCount * m_descriptorSize);
    return { handle.ptr };
}

template <typename T>
T UploadBufferDX12::GetHandle() const
{
    return std::get<T>(m_handle);
}
}