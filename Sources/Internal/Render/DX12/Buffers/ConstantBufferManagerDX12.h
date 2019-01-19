//
// Copyright (C) Aleksandr Vorontcov 2018.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <map>
#include <vector>
#include <windows.h>

#include "Render/RendererPublic.h"
#include "Render/DX12/Buffers/UploadBufferDX12.h"

namespace Kioto::Renderer
{
class ConstantBuffer;
class Material;
struct StateDX;

class ConstantBufferManagerDX12
{
public:
    ConstantBufferManagerDX12();
    ~ConstantBufferManagerDX12();
    void RegisterMaterial(Material* material);
    void ProcessRegistrationQueue(const StateDX& state);
    void RegisterConstantBuffer(ConstantBuffer* buffer, ConstantBufferSetHandle bufferSetHandle); // [a_vorontcov] -1 for internal buffers.
    void QueueConstantBufferForUpdate(ConstantBuffer& buffer);
    void ProcessBufferUpdates(UINT frameIndex);

    UploadBufferDX12* FindBuffer(ConstantBufferHandle handle) const;
    const std::vector<UploadBufferDX12*>& FindBuffers(ConstantBufferSetHandle handle) const;

private:
    struct TempCBData
    {
        ConstantBufferHandle CBHandle = InvalidHandle;
        ConstantBufferSetHandle CBSetHandle = InvalidHandle;
        uint32 DataSize = 0;
        float32* Data = nullptr;

        TempCBData(ConstantBufferHandle cbHandle, ConstantBufferSetHandle setHandle, uint32 dataSize, float32* data)
            : CBHandle(cbHandle)
            , CBSetHandle(setHandle)
            , DataSize(dataSize)
            , Data(data)
        {
        }
    };

    std::map<ConstantBufferHandle, UploadBufferDX12*> m_constantBuffers;
    std::map<ConstantBufferHandle, UploadBufferDX12*> m_internalBuffers;
    std::map<ConstantBufferSetHandle, std::vector<UploadBufferDX12*>> m_constantBufferSets;
    std::vector<ConstantBuffer*> m_updateQueues;
    std::vector<ConstantBuffer*> m_buffersToResetUpdatedFramesCount;
    std::vector<TempCBData> m_registrationQueue;
};
}