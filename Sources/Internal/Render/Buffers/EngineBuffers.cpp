#include "stdafx.h"

#include "Render/Buffers/EngineBuffers.h"

#include "Render/ConstantBuffer.h"

namespace Kioto::Renderer::EngineBuffers
{
namespace
{
ConstantBuffer m_cameraBuffer("cbCameraBuffer", CameraBufferIndex, EngineBuffersSpace, sizeof(CbCameraBuffer));
ConstantBuffer m_timeBuffer("cbEngineBuffer", TimeBufferIndex, EngineBuffersSpace, sizeof(CbEngineBuffer));
}

void EngineBuffers::Init()
{
}

void GetTimeBufferCopy(ConstantBuffer& target)
{
    m_timeBuffer.MakeShallowCopy(target, false);
    target.Reallocate();
}

void GetCameraBufferCopy(ConstantBuffer& target)
{
    m_cameraBuffer.MakeShallowCopy(target, false);
    target.Reallocate();
}
}