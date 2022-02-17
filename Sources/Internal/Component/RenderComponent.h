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

    bool GetCastShadows() const;
    void SetCastShadows(bool castShadows);

    bool GetIsVisible() const;
    void SetIsVisible(bool isVisible);

    void SetRenderObject(Renderer::RenderObject* renderObject);
    Renderer::RenderObject* GetRenderObject() const;

    void Serialize(YAML::Emitter& out) const override;
    void Deserialize(const YAML::Node& in) override;

private:
    std::string mMaterialPath = "";
    std::string mMeshPath = "";

    Renderer::RenderObject* mRenderObject = nullptr;
};
REGISTER_COMPONENT(RenderComponent);

inline void RenderComponent::SetMaterial(const std::string& path)
{
    mMaterialPath = path;
}

inline void RenderComponent::SetMesh(const std::string& path)
{
    mMeshPath = path;
}

inline const std::string& RenderComponent::GetMaterial() const
{
    return mMaterialPath;
}

inline const std::string& RenderComponent::GetMesh() const
{
    return mMeshPath;
}

inline void RenderComponent::SetRenderObject(Renderer::RenderObject* renderObject)
{
    mRenderObject = renderObject;
}

inline Renderer::RenderObject* RenderComponent::GetRenderObject() const
{
    return mRenderObject;
}

}