#pragma once

#include <map>
#include <vector>

#include "Core/WindowsCore.h"
#include "Render/RendererPublic.h"
#include "Render/DX12/Buffers/UploadBufferDX12.h"

namespace Kioto::Renderer
{
class ConstantBuffer;
class RenderObject;
struct StateDX;

class ConstantBufferManagerDX12
{
public:
    ConstantBufferManagerDX12();
    ~ConstantBufferManagerDX12();
    void RegisterRenderObject(RenderObject& renderObject);
    void ProcessRegistrationQueue(const StateDX& state);
    void RegisterConstantBuffer(ConstantBuffer* buffer); // [a_vorontcov] -1 for internal buffers.
    void QueueConstantBufferForUpdate(ConstantBuffer& buffer);
    void ProcessBufferUpdates(UINT frameIndex);

    UploadBufferDX12* FindBuffer(ConstantBufferHandle handle) const;

private:
    struct TempCBData
    {
        ConstantBufferHandle CBHandle = InvalidHandle;
        uint32 ElementSize = 0;
        uint32 ElementsCount = 0;
        byte* Data = nullptr;

        TempCBData(ConstantBufferHandle cbHandle, uint32 elementSize, uint32 elementsCount, byte* data)
            : CBHandle(cbHandle)
            , ElementSize(elementSize)
            , ElementsCount(elementsCount)
            , Data(data)
        {
        }
    };

    std::map<ConstantBufferHandle, UploadBufferDX12*> mConstantBuffers;
    std::map<ConstantBufferHandle, UploadBufferDX12*> mInternalBuffers;
    std::vector<ConstantBuffer*> mUpdateQueues;
    std::vector<ConstantBuffer*> mBuffersToResetUpdatedFramesCount;
    std::vector<TempCBData> mRegistrationQueue;
};
}