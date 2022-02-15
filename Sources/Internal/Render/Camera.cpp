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
        EngineBuffers::GetCameraBufferCopy(mCameraBuffer); // [a_vorontcov] TODO: Reeeeeeeeeealy don't like it here. Create some kind of manager.
        RegisterConstantBuffer(mCameraBuffer);
    }
}

void Camera::UpdateConstantBuffer()
{
    assert(mCameraBuffer.GetHandle() != InvalidHandle && "Camera buffer was not created for this camera");
    mCbCamera.ViewProjection = mVp.GetForGPU();
    mCbCamera.View = mView.GetForGPU();
    mCbCamera.ViewDirection = mToWorld.GetForward();
    mCbCamera.CamWorldPosition = mToWorld.GetTranslation();
    mCameraBuffer.Set(mCbCamera);
}

}