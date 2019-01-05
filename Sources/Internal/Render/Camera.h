//
// Copyright (C) Aleksandr Vorontcov. 2019
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/Core.h"
#include "Core/CoreTypes.h"
#include "Math/Matrix4.h"
#include "Render/RendererPublic.h"
#include "Render/ConstantBuffer.h"

namespace Kioto::Renderer
{
class Camera
{
public:
    ConstantBufferHandle GetConstantBufferHandle() const
    {
        return m_cameraBuffer.GetHandle();
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
    /// Set if camera is main camera.
    ///
    void SetIsMain(bool isMain);
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
    ///
    /// Get height of near clipping plane.
    ///
    float32 GetNearPlaneHeight() const;
    ///
    /// Get height of far clipping plane.
    ///
    float32 GetFarPlaneHeight() const;
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

private:
    ConstantBuffer m_cameraBuffer;

    Matrix4 m_view = Matrix4::Identity;
    Matrix4 m_projection = Matrix4::Identity;
    Matrix4 m_VP = Matrix4::Identity;
    bool m_isProjDirty = true;
    float32 m_fovY = Math::DegToRad(60.0f);
    float32 m_foxX = -1.0f;
    float32 m_nearPlane = 0.01f;
    float32 m_farPlane = 100.0f;
    float32 m_nearPlaneHeight = 0.0f;
    float32 m_farPlaneHeight = 0.0f;
    float32 m_aspect = 1.0f;
    bool m_isOrtho = false;
};

inline void Camera::SetFovY(float32 fovY)
{
    m_fovY = fovY;
    m_isProjDirty = true;
}

inline void Camera::SetFovYDeg(float32 fovY)
{
    m_fovY = Math::DegToRad(fovY);
    m_isProjDirty = true;
}

inline void Camera::SetNearPlane(float32 nearPlane)
{
    m_nearPlane = nearPlane;
    m_isProjDirty = true;
}

inline void Camera::SetFarPlane(float32 farPlane)
{
    m_farPlane = farPlane;
    m_isProjDirty = true;
}

inline void Camera::SetAspect(float32 aspect)
{
    m_aspect = aspect;
    m_isProjDirty = true;
}

inline void Camera::SetOrthographic(bool ortho)
{
    m_isOrtho = ortho;
    m_isProjDirty = true;
}

inline float32 Camera::GetFovY() const
{
    return m_fovY;
}

inline float32 Camera::GetNearPlane() const
{
    return m_nearPlane;
}

inline float32 Camera::GetFarPlane() const
{
    return m_farPlane;
}

inline float32 Camera::GetAspect() const
{
    return m_aspect;
}

inline bool Camera::GetOrthographic() const
{
    return m_isOrtho;
}

inline float32 Camera::GetNearPlaneHeight() const
{
    return m_nearPlane;
}
inline float32 Camera::GetFarPlaneHeight() const
{
    return m_farPlane;
}

inline Matrix4 Camera::GetView() const
{
    return m_view;
}

inline Matrix4 Camera::GetProjection() const
{
    return m_projection;
}

inline Matrix4 Camera::GetVP() const
{
    return m_VP;
}
}
