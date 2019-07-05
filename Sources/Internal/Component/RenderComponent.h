//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/CoreHelpers.h"
#include "Core/ECS/Component.h"

#include "Render/Material.h"
#include "Render/Geometry/Mesh.h"

namespace Kioto
{
namespace Renderer
{
class RenderObject;
}

class RenderComponent : public Component
{
    DECLARE_COMPONENT(RenderComponent);

public:
    KIOTO_API RenderComponent() = default;
    KIOTO_API ~RenderComponent() = default;

    Component* Clone() const override;

    void SetMaterial(const std::string& path);
    void SetMesh(const std::string& path);

    const std::string& GetMaterial() const;
    const std::string& GetMesh() const;

    void SetRenderObject(Renderer::RenderObject* renderObject);
    Renderer::RenderObject* GetRenderObject() const;

    void Save(YAML::Emitter& out) const override;

private:
    std::string m_materialPath = "";
    std::string m_meshPath = "";

    Renderer::RenderObject* m_renderObject = nullptr;
};
REGISTER_COMPONENT(RenderComponent);

inline void RenderComponent::SetMaterial(const std::string& path)
{
    m_materialPath = path;
}

inline void RenderComponent::SetMesh(const std::string& path)
{
    m_meshPath = path;
}

inline const std::string& RenderComponent::GetMaterial() const
{
    return m_materialPath;
}

inline const std::string& RenderComponent::GetMesh() const
{
    return m_meshPath;
}

inline void RenderComponent::SetRenderObject(Renderer::RenderObject* renderObject)
{
    m_renderObject = renderObject;
}

inline Renderer::RenderObject* RenderComponent::GetRenderObject() const
{
    return m_renderObject;
}

}