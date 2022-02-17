#pragma once

#include "Core/Core.h"
#include "Core/CoreTypes.h"
#include "Math/Matrix4.h"
#include "Render/ConstantBuffer.h"
#include "Render/Buffers/EngineBuffers.h"
#include "Render/RendererPublic.h"

#include "Render/Shaders/autogen/CommonStructures.h"

namespace Kioto::Renderer
{
class Camera
{
public:
    Camera(bool createBuffer = false);

    ConstantBufferHandle GetConstantBufferHandle() const
    {
        return mCameraBuffer.GetHandle();
    }

    const ConstantBuffer& GetConstantBuffer() const
    {
        return mCameraBuffer;
    }

    ///
    /// Set camera field of view in Y in radians.
    ///
    void SetFovY(float32 fovY);

    ///
    /// Set camera field of view in Y in degrees.
    ///
    void SetFovYDeg(float32 fovY);

    ///
    /// Set camera distance to near plane.
    ///
    void SetNearPlane(float32 nearPlane);

    ///
    /// Set camera distance to far plane.
    ///
    void SetFarPlane(float32 farPlane);

    ///
    /// Set camera aspect ratio (width/height).
    ///
    void SetAspect(float32 aspect);

    ///
    /// Set if camera is orthographic.
    ///
    void SetOrthographic(bool ortho);

    ///
    /// Get camera field of view in Y in radians.
    ///
    float32 GetFovY() const;

    ///
    /// Get distance to near clipping plane.
    ///
    float32 GetNearPlane() const;

    ///
    /// Get distance to far clipping plane.
    ///
    float32 GetFarPlane() const;

    ///
    /// Get camera aspect ratio (width / height).
    ///
    float32 GetAspect() const;

    ///
    /// Get if camera is orthographic.
    ///
    bool GetOrthographic() const;

    float32 GetOrthoHeight() const;
    void SetOrthoHeight(float32 h);

    float32 GetOrthoWidth() const;
    void SetOrthoWidth(float32 w);

    ///
    /// Get height of near clipping plane.
    ///
    float32 GetNearPlaneHeight() const;

    ///
    /// Get height of far clipping plane.
    ///
    float32 GetFarPlaneHeight() const;

    void SetView(const Matrix4& view);

    void SetToWorld(const Matrix4& toWorld);

    ///
    /// Get camera view matrix.
    ///
    Matrix4 GetView() const;

    ///
    /// Get camera projection matrix.
    ///
    Matrix4 GetProjection() const;

    ///
    /// Get camera (view * projection) matrix.
    ///
    Matrix4 GetVP() const;

    Matrix4 GetToWorld() const;

    bool GetIsProjectionDirty() const
    {
        return mIsProjDirty;
    }

    void UpdateProjectionMatrix();

    void UpdateViewProjectionMatrix();

    void UpdateConstantBuffer();

    void BuildProjection(float32 fovY, float32 aspect, float32 zNear, float32 zFar);
    void BuildOrtho(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);

private:
    void BuildProjectionInternal(float32 fovY, float32 aspect, float32 zNear, float32 zFar);
    void BuildOrthoInternal(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);

    SInp::CbCamera mCbCamera;
    ConstantBuffer mCameraBuffer; // [a_vorontcov] TODO: Really don't like it here.

    Matrix4 mToWorld = Matrix4::Identity;
    Matrix4 mView = Matrix4::Identity;
    Matrix4 mProjection = Matrix4::Identity;
    Matrix4 mVp = Matrix4::Identity;
    bool mIsProjDirty = true;
    float32 mFovY = Math::DegToRad(60.0f);
    float32 mFovX = -1.0f;
    float32 mNearPlane = 0.01f;
    float32 mFarPlane = 100.0f;
    float32 mNearPlaneHeight = 0.0f;
    float32 mFarPlaneHeight = 0.0f;

    // [a_vorontcov] Merge the setting with perspective camera settings (m_<near | far>PlaneHeight for example)
    // but now this way it seems to be more controllable
    float32 mOrthoWidth = 3.0f;
    float32 mOrthoHeight = 3.0f;

    float32 mAspect = 1.0f;
    bool mIsOrtho = false;
};


inline void Camera::SetToWorld(const Matrix4& toWorld)
{
    mToWorld = toWorld;
}


inline void Camera::SetFovY(float32 fovY)
{
    mFovY = fovY;
    mIsProjDirty = true;
}

inline void Camera::SetFovYDeg(float32 fovY)
{
    mFovY = Math::DegToRad(fovY);
    mIsProjDirty = true;
}

inline void Camera::SetNearPlane(float32 nearPlane)
{
    mNearPlane = nearPlane;
    mIsProjDirty = true;
}

inline void Camera::SetFarPlane(float32 farPlane)
{
    mFarPlane = farPlane;
    mIsProjDirty = true;
}

inline void Camera::SetAspect(float32 aspect)
{
    mAspect = aspect;
    mIsProjDirty = true;
}

inline void Camera::SetOrthographic(bool ortho)
{
    mIsOrtho = ortho;
    mIsProjDirty = true;
}

inline float32 Camera::GetFovY() const
{
    return mFovY;
}

inline float32 Camera::GetNearPlane() const
{
    return mNearPlane;
}

inline float32 Camera::GetFarPlane() const
{
    return mFarPlane;
}

inline float32 Camera::GetAspect() const
{
    return mAspect;
}

inline bool Camera::GetOrthographic() const
{
    return mIsOrtho;
}

inline float32 Camera::GetOrthoHeight() const
{
    return mOrthoHeight;
}

inline void Camera::SetOrthoHeight(float32 h)
{
    mOrthoHeight = h;
    mIsProjDirty = true;
}

inline float32 Camera::GetOrthoWidth() const
{
    return mOrthoWidth;
}

inline void Camera::SetOrthoWidth(float32 w)
{
    mOrthoWidth = w;
    mIsProjDirty = true;
}

inline float32 Camera::GetNearPlaneHeight() const
{
    return mNearPlane;
}
inline float32 Camera::GetFarPlaneHeight() const
{
    return mFarPlane;
}

inline Matrix4 Camera::GetView() const
{
    return mView;
}

inline Matrix4 Camera::GetProjection() const
{
    return mProjection;
}

inline Matrix4 Camera::GetVP() const
{
    return mVp;
}

inline Matrix4 Camera::GetToWorld() const
{
    return mToWorld;
}

inline void Camera::UpdateProjectionMatrix()
{
    if (!mIsOrtho)
    {
        BuildProjectionInternal(mFovY, mAspect, mNearPlane, mFarPlane);
    }
    else
    {
        assert(mOrthoHeight > 0.0f && mOrthoWidth > 0.0f && "Invalid params for an ortho camera");
        float32 halfHeight = mOrthoHeight * 0.5f;
        float32 halfWidth = mOrthoWidth * 0.5f;
        BuildOrthoInternal(-halfWidth, halfWidth, -halfHeight, halfHeight, mNearPlane, mFarPlane);
    }
}

inline void Camera::UpdateViewProjectionMatrix()
{
    mVp = mView * mProjection;
}

inline void Camera::SetView(const Matrix4& view)
{
    mView = view;
}

inline void Camera::BuildProjection(float32 fovY, float32 aspect, float32 zNear, float32 zFar)
{
    mFovY = fovY;
    mNearPlane = zNear;
    mFarPlane = zFar;
    mAspect = aspect;

    BuildProjectionInternal(mFovY, mAspect, mNearPlane, mFarPlane);
}

inline void Camera::BuildOrtho(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
{
    mOrthoWidth = left * 2;
    mOrthoHeight = top * 2;
    mNearPlane = zNear;
    mFarPlane = zFar;

    BuildOrthoInternal(left, right, bottom, top, zNear, zFar);
}

inline void Camera::BuildProjectionInternal(float32 fovY, float32 aspect, float32 zNear, float32 zFar)
{
    mNearPlaneHeight = 2.0f * mNearPlane * std::tan(0.5f * mFovY);
    mFarPlaneHeight = 2.0f * mFarPlane * std::tan(0.5f * mFovY);

    float32 halfWidth = 0.5f * mAspect * mNearPlaneHeight;
    mFovX = 2.0f * atan(halfWidth / mNearPlane);

    mProjection = Matrix4::BuildProjectionFov(GetFovY(), GetAspect(), GetNearPlane(), GetFarPlane());

    mIsProjDirty = false;
}

inline void Camera::BuildOrthoInternal(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
{
    mProjection = Matrix4::BuildOrtho(left, right, bottom, top, zNear, zFar);

    mIsProjDirty = false;
}
}
