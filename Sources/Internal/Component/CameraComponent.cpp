//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Component/CameraComponent.h"

#include "Core/ECS/Entity.h"

namespace Kioto
{
CameraComponent::CameraComponent(float32 fovY, float32 aspect, float32 nearPlane, float32 farPlane)
{
    m_camera.SetFovY(fovY);
    m_camera.SetAspect(aspect);
    m_camera.SetNearPlane(nearPlane);
    m_camera.SetFarPlane(farPlane);
}

Component* CameraComponent::Clone() const
{
    CameraComponent* newComponent = new CameraComponent();
    newComponent->m_camera = m_camera;
    newComponent->m_hdr = m_hdr;
    return newComponent;
}

void CameraComponent::SetEntity(Entity* entity)
{
    Component::SetEntity(entity);
    m_transform = GetEntity()->GetTransform();
}
}