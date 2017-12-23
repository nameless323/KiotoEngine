//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "Core/CoreTypes.h"
#include "Core/CoreHelpers.h"
#include "Core/ECS/Component.h"

#include "Render/Material.h"
#include "Render/Geometry/Mesh.h"

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
    void SetMaterial(Material* material);

    void SetMesh(const Mesh& mesh);
    void SetMesh(Mesh* mesh);

private:
    Material * m_material = nullptr;
    Mesh* m_mesh = nullptr;
};

inline void RenderComponent::SetMaterial(const std::string& path)
{
    SafeDelete(m_material); // [a_vorontsov] Too fragile. TODO: all material work via asset system.
    m_material = new Material(path);
}

inline void RenderComponent::SetMaterial(Material* material)
{
    if (m_material == material)
        return;
    SafeDelete(m_material);
    m_material = material;
}

inline void RenderComponent::SetMesh(const Mesh& mesh)
{
    if (m_mesh == &mesh)
        return;
    SafeDelete(m_mesh);
    m_mesh = new Mesh(mesh);
}

inline void RenderComponent::SetMesh(Mesh* mesh)
{
    if (m_mesh == mesh)
        return;
    SafeDelete(m_mesh);
    m_mesh = mesh;
}

}