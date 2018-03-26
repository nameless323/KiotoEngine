//
// Copyright (C) Alexandr Vorontsov 2018.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <map>
#include <vector>
#include <windows.h>

#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class ConstantBuffer;
class UploadBufferDX12;
class Material;

class ConstantBufferManagerDX12
{
public:
    ConstantBufferManagerDX12();
    ~ConstantBufferManagerDX12();
    void RegisterMaterial(const Material* material);
    void RegisterConstantBuffer(ConstantBuffer* buffer);
    void QueueConstantBufferForUpdate(const ConstantBuffer& buffer);
    void ProcessBufferUpdates(UINT frameIndex);

    UploadBufferDX12* FindBuffer(ConstantBufferHandle handle) const;

private:
    std::map<ConstantBufferHandle, UploadBufferDX12*> m_constantBuffers;
    std::vector<ConstantBuffer*> m_updateQueues;
};
}