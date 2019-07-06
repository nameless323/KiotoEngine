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
    out << YAML::Key << "Cam.View" << YAML::Value << m_camera.GetView();
    out << YAML::Key << "Cam.FOV_Y" << YAML::Value << m_camera.GetFovY();
    out << YAML::Key << "Cam.Near" << YAML::Value << m_camera.GetNearPlane();
    out << YAML::Key << "Cam.Far" << YAML::Value << m_camera.GetFarPlane();
    out << YAML::Key << "Cam.Aspect" << YAML::Value << m_camera.GetAspect();
    out << YAML::Key << "Cam.Ortho" << YAML::Value << m_camera.GetOrthographic();
}

void CameraComponent::Load(const YAML::Node& in)
{
    if (in["Cam.View"] != nullptr)
        m_camera.SetView(in["Cam.View"].as<Matrix4>());
    if (in["Cam.FOV_Y"] != nullptr)
        m_camera.SetFovY(in["Cam.FOV_Y"].as<float32>());
    if (in["Cam.Near"] != nullptr)
        m_camera.SetNearPlane(in["Cam.Near"].as<float32>());
    if (in["Cam.Far"] != nullptr)
        m_camera.SetFarPlane(in["Cam.Far"].as<float32>());
    if (in["Cam.Aspect"] != nullptr)
        m_camera.SetAspect(in["Cam.Aspect"].as<float32>());
    if (in["Cam.Ortho"] != nullptr)
        m_camera.SetOrthographic(in["Cam.Ortho"].as<bool>());
}
}