#include "Render/Buffers/EngineBuffers.h"

#include "Render/ConstantBuffer.h"

#include "Render/Shaders/autogen/CommonStructures.h"

namespace Kioto::Renderer::EngineBuffers
{
namespace
{
ConstantBuffer m_cameraBuffer("cbCamera", CameraBufferIndex, EngineBuffersSpace, sizeof(SInp::CbCamera));
ConstantBuffer m_timeBuffer("cbEngine", TimeBufferIndex, EngineBuffersSpace, sizeof(SInp::CbEngine), 1, true);
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

ConstantBuffer& GetTimeBuffer()
{
    return m_timeBuffer;
}

}