//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Core/CoreTypes.h"
#include "Core/CoreHelpers.h"
#include "Core/ECS/Component.h"

#include "Render/Material.h"
#include "Render/Geometry/Mesh2.h"

#pragma once

namespace Kioto
{

class RenderComponent : public Component
{
    DECLARE_COMPONENT(RenderComponent);

public:
    KIOTO_API RenderComponent() = default;
    KIOTO_API ~RenderComponent();

    Component* Clone() const override;

    void SetMaterial(const std::string& path);
    void SetMaterial(Renderer::Material* material);

    void SetMesh(const Mesh2& mesh);
    void SetMesh(Mesh2* mesh);

private:
    Renderer::Material * m_material = nullptr;
    Mesh2* m_mesh = nullptr;
};

inline void RenderComponent::SetMaterial(const std::string& path)
{
    SafeDelete(m_material); // [a_vorontsov] Too fragile. TODO: all material work via asset system.
    m_material = new Renderer::Material(path);
}

inline void RenderComponent::SetMaterial(Renderer::Material* material)
{
    if (m_material == material)
        return;
    SafeDelete(m_material);
    m_material = material;
}

inline void RenderComponent::SetMesh(const Mesh2& mesh)
{
    if (m_mesh == &mesh)
        return;
    SafeDelete(m_mesh);
    m_mesh = new Mesh2(mesh);
}

inline void RenderComponent::SetMesh(Mesh2* mesh)
{
    if (m_mesh == mesh)
        return;
    SafeDelete(m_mesh);
    m_mesh = mesh;
}

}