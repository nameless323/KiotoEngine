//
// Copyright (C) Aleksandr Vorontcov 2018.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Buffers/ConstantBufferManagerDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/Material.h"

namespace Kioto::Renderer
{

ConstantBufferManagerDX12::ConstantBufferManagerDX12()
{
    m_updateQueues.reserve(128);
    m_registrationQueue.reserve(128);
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
        RegisterConstantBuffer(&material->GetShaderData().constantBuffers[i], setHandle);
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

void ConstantBufferManagerDX12::RegisterConstantBuffer(ConstantBuffer* buffer, ConstantBufferSetHandle bufferSetHandle)
{
    if (buffer->GetHandle() != InvalidHandle)
        return;
    auto it = m_constantBuffers.find(buffer->GetHandle()); // [a_vorontcov] We have this buffer registred.
    if (it != m_constantBuffers.cend())
        return;
    for (auto& tmpBuf : m_registrationQueue)
    {
        if (tmpBuf.CBHandle == buffer->GetHandle())
            return;
    }

    ConstantBufferHandle bufHandle = GetNewHandle();
    buffer->SetHandle(bufHandle);
    m_registrationQueue.emplace_back(bufHandle, bufferSetHandle, buffer->GetDataSize(), buffer->GetBufferData());
    QueueConstantBufferForUpdate(*buffer);
}

void ConstantBufferManagerDX12::QueueConstantBufferForUpdate(ConstantBuffer& buffer)
{
    const auto it = std::find(m_updateQueues.cbegin(), m_updateQueues.cend(), &buffer);
    if (it != m_updateQueues.cend() || buffer.GetHandle() == InvalidHandle)
        return;
    m_updateQueues.push_back(&buffer);
}

void ConstantBufferManagerDX12::ProcessBufferUpdates(UINT frameIndex)
{
    for (auto& cb : m_updateQueues)
    {
        UploadBufferDX12* uploadBuf = FindBuffer(cb->GetHandle());

        assert(cb->GetIsComposed());
        if (uploadBuf == nullptr)
            assert(false);
        uploadBuf->UploadData(frameIndex, cb->GetBufferData());
    }
    m_updateQueues.clear();
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