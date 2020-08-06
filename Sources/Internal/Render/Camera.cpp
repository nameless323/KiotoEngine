#include "stdafx.h"

#include "Render/Camera.h"

#include "Render/RendererPublic.h"
#include "Render/Renderer.h"
#include "Render/Buffers/EngineBuffers.h"

namespace Kioto::Renderer
{
Camera::Camera(bool createBuffer)
{
    if (createBuffer)
    {
        EngineBuffers::GetCameraBufferCopy(m_cameraBuffer); // [a_vorontcov] TODO: Reeeeeeeeeealy don't like it here. Create some kind of manager.
        Renderer::RegisterConstantBuffer(m_cameraBuffer);
    }
}

void Camera::UpdateConstantBuffer()
{
    assert(m_cameraBuffer.GetHandle() != InvalidHandle && "Camera buffer was not created for this camera");
    m_cbCamera.ViewProjection = m_VP.GetForGPU();
    m_cbCamera.View = m_view.GetForGPU();
    //m_cbCamera.ViewDirection = ViewDirection;
    m_cameraBuffer.Set(m_cbCamera);
}
}