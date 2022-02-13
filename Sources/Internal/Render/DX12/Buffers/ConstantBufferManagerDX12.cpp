#include "Render/DX12/Buffers/ConstantBufferManagerDX12.h"
#include "Render/DX12/StateDX.h"
#include "Render/RenderObject.h"

namespace Kioto::Renderer
{
ConstantBufferManagerDX12::ConstantBufferManagerDX12()
{
    mUpdateQueues.reserve(128);
    mBuffersToResetUpdatedFramesCount.reserve(128);
    mRegistrationQueue.reserve(128);
}

ConstantBufferManagerDX12::~ConstantBufferManagerDX12()
{
    for (auto& buf : mConstantBuffers)
        SafeDelete(buf.second);
    mConstantBuffers.clear();

    for (auto& buf : mInternalBuffers)
        SafeDelete(buf.second);
    mInternalBuffers.clear();
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
    for (auto& tmpBuf : mRegistrationQueue)
    {
        UploadBufferDX12* buf = new UploadBufferDX12(state, tmpBuf.Data, tmpBuf.ElementSize, tmpBuf.ElementsCount, true);
        SafeDelete(mConstantBuffers[tmpBuf.CBHandle]);
        mConstantBuffers[tmpBuf.CBHandle] = buf;
    }
    mRegistrationQueue.clear();
}

void ConstantBufferManagerDX12::RegisterConstantBuffer(ConstantBuffer* buffer)
{
    if (buffer->GetHandle() != InvalidHandle)
        return;
    auto it = mConstantBuffers.find(buffer->GetHandle()); // [a_vorontcov] We have this buffer registred.
    if (it != mConstantBuffers.cend())
        return;
    for (auto& tmpBuf : mRegistrationQueue)
    {
        if (tmpBuf.CBHandle == buffer->GetHandle())
            return;
    }

    ConstantBufferHandle bufHandle = GetNewHandle();
    buffer->SetHandle(bufHandle);
    mRegistrationQueue.emplace_back(bufHandle, buffer->GetElemSize(), buffer->GetElemCount(), buffer->GetBufferData());
    QueueConstantBufferForUpdate(*buffer);
}

void ConstantBufferManagerDX12::QueueConstantBufferForUpdate(ConstantBuffer& buffer)
{
    if (buffer.GetHandle() == InvalidHandle)
        return;

    auto itr = std::find(mBuffersToResetUpdatedFramesCount.cbegin(), mBuffersToResetUpdatedFramesCount.cend(), &buffer);
    if (itr == mBuffersToResetUpdatedFramesCount.cend())
        mBuffersToResetUpdatedFramesCount.push_back(&buffer); // [a_vorontcov] TODO: Very questionable.

    auto it = std::find_if(mUpdateQueues.cbegin(), mUpdateQueues.cend(), [&buffer](const ConstantBuffer* b) { return b->GetHandle() == buffer.GetHandle(); });
    if (it != mUpdateQueues.cend())
        return;

    mUpdateQueues.push_back(&buffer);
}

void ConstantBufferManagerDX12::ProcessBufferUpdates(UINT frameIndex)
{
    std::vector<ConstantBuffer*> intermediateQueue; // [a_vorontcov] TODO: Too ugly. Rethink.
    intermediateQueue.reserve(mUpdateQueues.size());
    for (auto& cb : mUpdateQueues)
    {
        UploadBufferDX12* uploadBuf = FindBuffer(cb->GetHandle());

        assert(cb->IsAllocated());
        if (uploadBuf == nullptr)
        {
            assert(false);
            continue;
        }

        auto it = std::find(mBuffersToResetUpdatedFramesCount.cbegin(), mBuffersToResetUpdatedFramesCount.cend(), cb);
        if (it != mBuffersToResetUpdatedFramesCount.cend())
            uploadBuf->ResetUpdatedFramesCount();

        uploadBuf->UploadData(frameIndex, cb->GetBufferData());
        uploadBuf->IncrementUpdatedFramesCount();
        if (!uploadBuf->IsUpdated())
            intermediateQueue.push_back(cb);
    }
    mBuffersToResetUpdatedFramesCount.clear();
    mUpdateQueues.clear();
    mUpdateQueues = std::move(intermediateQueue);
}

UploadBufferDX12* ConstantBufferManagerDX12::FindBuffer(ConstantBufferHandle handle) const
{
    auto it = mConstantBuffers.find(handle);
    if (it == mConstantBuffers.end())
        return nullptr;
    return it->second;
}
}