#include "stdafx.h"

#include "Render/DX12/Buffers/ConstantBufferManagerDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/RenderObject.h"

namespace Kioto::Renderer
{
ConstantBufferManagerDX12::ConstantBufferManagerDX12()
{
    m_updateQueues.reserve(128);
    m_buffersToResetUpdatedFramesCount.reserve(128);
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

void ConstantBufferManagerDX12::RegisterRenderObject(RenderObject& renderObject)
{
    std::unordered_map<std::string, RenderObjectBufferLayout>& bufferLayouts = renderObject.GetBuffersLayouts();
    for (auto& layoutElem : bufferLayouts)
    {
        RenderObjectBufferLayout& bufferLayout = layoutElem.second;
        for (size_t i = 0; i < bufferLayout.size(); ++i)
        {
            if (bufferLayout[i].IsPerObjectBuffer())
                RegisterConstantBuffer(&bufferLayout[i]);
        }
    }
}

void ConstantBufferManagerDX12::ProcessRegistrationQueue(const StateDX& state)
{
    for (auto& tmpBuf : m_registrationQueue)
    {
        UploadBufferDX12* buf = new UploadBufferDX12(state, tmpBuf.Data, tmpBuf.ElementSize, tmpBuf.ElementsCount, true);
        SafeDelete(m_constantBuffers[tmpBuf.CBHandle]);
        m_constantBuffers[tmpBuf.CBHandle] = buf;
    }
    m_registrationQueue.clear();
}

void ConstantBufferManagerDX12::RegisterConstantBuffer(ConstantBuffer* buffer)
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
    m_registrationQueue.emplace_back(bufHandle, buffer->GetElemSize(), buffer->GetElemCount(), buffer->GetBufferData());
    QueueConstantBufferForUpdate(*buffer);
}

void ConstantBufferManagerDX12::QueueConstantBufferForUpdate(ConstantBuffer& buffer)
{
    if (buffer.GetHandle() == InvalidHandle)
        return;

    auto itr = std::find(m_buffersToResetUpdatedFramesCount.cbegin(), m_buffersToResetUpdatedFramesCount.cend(), &buffer);
    if (itr == m_buffersToResetUpdatedFramesCount.cend())
        m_buffersToResetUpdatedFramesCount.push_back(&buffer); // [a_vorontcov] TODO: Very questionable.

    auto it = std::find_if(m_updateQueues.cbegin(), m_updateQueues.cend(), [&buffer](const ConstantBuffer* b) { return b->GetHandle() == buffer.GetHandle(); });
    if (it != m_updateQueues.cend())
        return;

    m_updateQueues.push_back(&buffer);
}

void ConstantBufferManagerDX12::ProcessBufferUpdates(UINT frameIndex)
{
    std::vector<ConstantBuffer*> intermediateQueue; // [a_vorontcov] TODO: Too ugly. Rethink.
    intermediateQueue.reserve(m_updateQueues.size());
    for (auto& cb : m_updateQueues)
    {
        UploadBufferDX12* uploadBuf = FindBuffer(cb->GetHandle());

        assert(cb->IsAllocated());
        if (uploadBuf == nullptr)
        {
            assert(false);
            continue;
        }

        auto it = std::find(m_buffersToResetUpdatedFramesCount.cbegin(), m_buffersToResetUpdatedFramesCount.cend(), cb);
        if (it != m_buffersToResetUpdatedFramesCount.cend())
            uploadBuf->ResetUpdatedFramesCount();

        uploadBuf->UploadData(frameIndex, cb->GetBufferData());
        uploadBuf->IncrementUpdatedFramesCount();
        if (!uploadBuf->IsUpdated())
            intermediateQueue.push_back(cb);
    }
    m_buffersToResetUpdatedFramesCount.clear();
    m_updateQueues.clear();
    m_updateQueues = std::move(intermediateQueue);
}

UploadBufferDX12* ConstantBufferManagerDX12::FindBuffer(ConstantBufferHandle handle) const
{
    auto it = m_constantBuffers.find(handle);
    if (it == m_constantBuffers.end())
        return nullptr;
    return it->second;
}
}