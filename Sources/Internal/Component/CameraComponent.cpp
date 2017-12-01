//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Component/CameraComponent.h"

#include "Core/ECS/Entity.h"

namespace Kioto
{

CameraComponent::CameraComponent()
{
    m_transform = GetEntity()->GetTransform();
}

CameraComponent::CameraComponent(float32 fovY, float32 aspect, float32 nearPlane, float32 farPlane)
{
    m_transform = GetEntity()->GetTransform();

    m_fovY = fovY;
    m_aspect = aspect;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
}

Component* CameraComponent::Clone() const
{
    CameraComponent* newComponent = new CameraComponent();
    newComponent->m_fovY = m_fovY;
    newComponent->m_aspect = m_aspect;
    newComponent->m_nearPlane = m_nearPlane;
    newComponent->m_farPlane = m_farPlane;
    newComponent->m_hdr = m_hdr;
    newComponent->m_isOrtho = m_isOrtho;
    return newComponent;
}

}