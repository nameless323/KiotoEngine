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
        return m_cameraBuffer.GetHandle();
    }

    const ConstantBuffer& GetConstantBuffer() const
    {
        return m_cameraBuffer;
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
        return m_isProjDirty;
    }

    void UpdateProjectionMatrix();

    void UpdateViewProjectionMatrix();

    void UpdateConstantBuffer();

    void BuildProjection(float32 fovY, float32 aspect, float32 zNear, float32 zFar);
    void BuildOrtho(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);

private:
    void BuildProjectionInternal(float32 fovY, float32 aspect, float32 zNear, float32 zFar);
    void BuildOrthoInternal(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar);

    SInp::CbCamera m_cbCamera;
    ConstantBuffer m_cameraBuffer; // [a_vorontcov] TODO: Really don't like it here.

    Matrix4 m_toWorld = Matrix4::Identity;
    Matrix4 m_view = Matrix4::Identity;
    Matrix4 m_projection = Matrix4::Identity;
    Matrix4 m_VP = Matrix4::Identity;
    bool m_isProjDirty = true;
    float32 m_fovY = Math::DegToRad(60.0f);
    float32 m_fovX = -1.0f;
    float32 m_nearPlane = 0.01f;
    float32 m_farPlane = 100.0f;
    float32 m_nearPlaneHeight = 0.0f;
    float32 m_farPlaneHeight = 0.0f;

    // [a_vorontcov] Merge the setting with perspective camera settings (m_<near | far>PlaneHeight for example)
    // but now this way it seems to be more controllable
    float32 m_orthoWidth = 3.0f;
    float32 m_orthoHeight = 3.0f;

    float32 m_aspect = 1.0f;
    bool m_isOrtho = false;
};


inline void Camera::SetToWorld(const Matrix4& toWorld)
{
    m_toWorld = toWorld;
}


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

inline float32 Camera::GetOrthoHeight() const
{
    return m_orthoHeight;
}

inline void Camera::SetOrthoHeight(float32 h)
{
    m_orthoHeight = h;
    m_isProjDirty = true;
}

inline float32 Camera::GetOrthoWidth() const
{
    return m_orthoWidth;
}

inline void Camera::SetOrthoWidth(float32 w)
{
    m_orthoWidth = w;
    m_isProjDirty = true;
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

inline Matrix4 Camera::GetToWorld() const
{
    return m_toWorld;
}

inline void Camera::UpdateProjectionMatrix()
{
    if (!m_isOrtho)
    {
        BuildProjectionInternal(m_fovY, m_aspect, m_nearPlane, m_farPlane);
    }
    else
    {
        assert(m_orthoHeight > 0.0f && m_orthoWidth > 0.0f && "Invalid params for an ortho camera");
        float32 halfHeight = m_orthoHeight * 0.5f;
        float32 halfWidth = m_orthoWidth * 0.5f;
        BuildOrthoInternal(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
    }
}

inline void Camera::UpdateViewProjectionMatrix()
{
    m_VP = m_view * m_projection;
}

inline void Camera::SetView(const Matrix4& view)
{
    m_view = view;
}

inline void Camera::BuildProjection(float32 fovY, float32 aspect, float32 zNear, float32 zFar)
{
    m_fovY = fovY;
    m_nearPlane = zNear;
    m_farPlane = zFar;
    m_aspect = aspect;

    BuildProjectionInternal(m_fovY, m_aspect, m_nearPlane, m_farPlane);
}

inline void Camera::BuildOrtho(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
{
    m_orthoWidth = left * 2;
    m_orthoHeight = top * 2;
    m_nearPlane = zNear;
    m_farPlane = zFar;

    BuildOrthoInternal(left, right, bottom, top, zNear, zFar);
}

inline void Camera::BuildProjectionInternal(float32 fovY, float32 aspect, float32 zNear, float32 zFar)
{
    m_nearPlaneHeight = 2.0f * m_nearPlane * std::tan(0.5f * m_fovY);
    m_farPlaneHeight = 2.0f * m_farPlane * std::tan(0.5f * m_fovY);

    float32 halfWidth = 0.5f * m_aspect * m_nearPlaneHeight;
    m_fovX = 2.0f * atan(halfWidth / m_nearPlane);

    m_projection = Matrix4::BuildProjectionFov(GetFovY(), GetAspect(), GetNearPlane(), GetFarPlane());

    m_isProjDirty = false;
}

inline void Camera::BuildOrthoInternal(float32 left, float32 right, float32 bottom, float32 top, float32 zNear, float32 zFar)
{
    m_projection = Matrix4::BuildOrtho(left, right, bottom, top, zNear, zFar);

    m_isProjDirty = false;
}
}
