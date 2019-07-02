//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Component/CameraComponent.h"

#include "Core/ECS/Entity.h"

#include "Core/Yaml/YamlParser.h"

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

void CameraComponent::Save(YAML::Emitter& out) const
{
    BEGIN_COMPONENT_SERIALIZARION();
    out << YAML::Key << "Cam.View" << YAML::Value << m_camera.GetView();
    out << YAML::Key << "Cam.Proj" << YAML::Value << m_camera.GetProjection();
    out << YAML::Key << "Cam.VP" << YAML::Value << m_camera.GetVP();
    out << YAML::Key << "Cam.FOV_Y" << YAML::Value << m_camera.GetFovY();
    out << YAML::Key << "Cam.Near" << YAML::Value << m_camera.GetNearPlane();
    out << YAML::Key << "Cam.Far" << YAML::Value << m_camera.GetFarPlane();
    out << YAML::Key << "Cam.NearHeight" << YAML::Value << m_camera.GetNearPlaneHeight();
    out << YAML::Key << "Cam.FarHeight" << YAML::Value << m_camera.GetFarPlaneHeight();
    out << YAML::Key << "Cam.Aspect" << YAML::Value << m_camera.GetAspect();
    out << YAML::Key << "Cam.Ortho" << YAML::Value << m_camera.GetOrthographic();
    END_COMPONENT_SERIALIZATION();
}
}