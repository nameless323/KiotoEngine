//
// Copyright (C) Alexandr Vorontsov 2018.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Buffers/ConstantBufferManagerDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/Material.h"

namespace Kioto::Renderer
{

ConstantBufferManagerDX12::ConstantBufferManagerDX12(uint32 frameCount) : m_frameCount(frameCount)
{
    m_updateQueues.reserve(128);
}

ConstantBufferManagerDX12::~ConstantBufferManagerDX12()
{
    for (auto& buf : m_constantBuffers)
        SafeDelete(buf.second);
    m_constantBuffers.clear();

    for (auto& buf : m_internalBuffers)
        SafeDelete(buf.second);
    m_internalBuffers.clear();
}

void ConstantBufferManagerDX12::RegisterMaterial(Material* material)
{
    auto it = m_constantBufferSets.find(material->GetShaderData().bufferSetHandle);
    if (it != m_constantBufferSets.end() && material->GetShaderData().bufferSetHandle != InvalidHandle)
        return;
    ConstantBufferSetHandle setHandle = GetNewHandle();
    material->GetShaderData().bufferSetHandle = setHandle;
    for (size_t i = 0; i < material->GetShaderData().constantBuffers.size(); ++i)
    {
        ConstantBufferHandle bufHandle = GetNewHandle();
        ConstantBuffer& buf = material->GetShaderData().constantBuffers[i];
        assert(buf.GetIsComposed());
        material->GetShaderData().constantBuffersHandles[i] = bufHandle;
        m_registrationQueue.emplace_back(bufHandle, setHandle, buf.GetDataSize(), buf.GetBufferData());
    }
}

void ConstantBufferManagerDX12::ProcessRegistrationQueue(const StateDX& state)
{
    for (auto& tmpBuf : m_registrationQueue)
    {
        UploadBufferDX12* buf = new UploadBufferDX12(StateDX::FrameCount, tmpBuf.Data, tmpBuf.DataSize, true, state.Device.Get());
        SafeDelete(m_constantBuffers[tmpBuf.CBHandle]);
        m_constantBuffers[tmpBuf.CBHandle] = buf;
        m_constantBufferSets[tmpBuf.CBSetHandle].push_back(buf);
    }
    m_registrationQueue.clear();
}

void ConstantBufferManagerDX12::RegisterConstantBuffer(ConstantBuffer* buffer)
{

}

void ConstantBufferManagerDX12::RegisterInternalConstantBuffer(ConstantBuffer* buffer)
{

}

void ConstantBufferManagerDX12::QueueConstantBufferForUpdate(const ConstantBuffer& buffer)
{

}

void ConstantBufferManagerDX12::ProcessBufferUpdates(UINT frameIndex)
{

}

UploadBufferDX12* ConstantBufferManagerDX12::FindBuffer(ConstantBufferHandle handle) const
{
    auto it = m_constantBuffers.find(handle);
    if (it == m_constantBuffers.end())
        return nullptr;
    return it->second;
}

const std::vector<UploadBufferDX12*>& ConstantBufferManagerDX12::FindBuffers(ConstantBufferSetHandle handle) const
{
    auto it = m_constantBufferSets.find(handle);
    return it->second;
}

}