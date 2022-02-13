#include "Component/RenderComponent.h"

#include "Core/Yaml/YamlParser.h"
#include "Render/RenderObject.h"

namespace Kioto
{
Component* RenderComponent::Clone() const
{
    return new RenderComponent();
}

bool RenderComponent::GetCastShadows() const
{
    return mRenderObject->GetCastShadow();
}

void RenderComponent::SetCastShadows(bool castShadows)
{
    mRenderObject->SetCastShadow(castShadows);
}

bool RenderComponent::GetIsVisible() const
{
    return mRenderObject->GetIsVisible();
}

void RenderComponent::SetIsVisible(bool isVisible)
{
    mRenderObject->SetIsVisible(isVisible);
}

void RenderComponent::Serialize(YAML::Emitter& out) const
{
    out << YAML::Key << "MatPath" << YAML::Value << mMaterialPath;
    out << YAML::Key << "MeshPath" << YAML::Value << mMeshPath;
}

void RenderComponent::Deserialize(const YAML::Node& in)
{
    if (in["MatPath"])
        mMaterialPath = in["MatPath"].as<std::string>();
    if (in["MeshPath"])
        mMeshPath = in["MeshPath"].as<std::string>();
}
}