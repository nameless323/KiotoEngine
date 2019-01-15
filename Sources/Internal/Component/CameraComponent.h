//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/Core.h"
#include "Core/CoreTypes.h"
#include "Core/ECS/Component.h"
#include "Math/Matrix4.h"
#include "Math/MathHelpers.h"
#include "Render/Camera.h"

namespace Kioto
{
class TransformComponent;

class CameraComponent : public Component
{
    DECLARE_COMPONENT(CameraComponent);

public:
    KIOTO_API CameraComponent() = default;
    ///
    /// Create camera component.
    /// fovY - field of view Y in radians.
    /// aspect - camera aspect ratio (width / height).
    /// nearPlane - distance to near plane.
    /// farPlane - distance to far plane.
    ///
    KIOTO_API CameraComponent(float32 fovY, float32 aspect, float32 nearPlane, float32 farPlane);
    KIOTO_API ~CameraComponent() = default;

    Component* Clone() const override;

    ///
    /// Get transform component attached to camera.
    ///
    const TransformComponent* GetTransform() const;
    
    ///
    /// Get if camera is main.
    ///
    bool GetIsMain() const;

    void SetIsMain(bool val)
    {
        m_isMainRT = val;
    }

    Renderer::Camera& GetCamera();

protected:
    void SetEntity(Entity* entity) override;

private:
    TransformComponent* m_transform = nullptr;
    bool m_hdr = true;
    bool m_isMainRT = false;

    Renderer::Camera m_camera{ true };

    friend class CameraSystem;
};

inline const TransformComponent* CameraComponent::GetTransform() const
{
    return m_transform;
}

inline bool CameraComponent::GetIsMain() const
{
    return m_isMainRT;
}

inline Renderer::Camera& CameraComponent::GetCamera()
{
    return m_camera;
}

}