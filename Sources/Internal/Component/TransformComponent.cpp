#include "Component/TransformComponent.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{
Component* TransformComponent::Clone() const
{
    TransformComponent* t = new TransformComponent();
    t->mToWorld = mToWorld;
    t->mToParent = mToParent;
    t->mToModel = mToModel;

    t->mChildren = mChildren;
    t->mParent = mParent;
    t->mWorldPosition = mWorldPosition;
    t->mWorldRotation = mWorldRotation;

    return t;
}

void TransformComponent::Serialize(YAML::Emitter& out) const
{
    using ::operator<<;
    out << YAML::Key << "WorldPosition" << YAML::Value << mWorldPosition;
    out << YAML::Key << "WorldRotation" << YAML::Value << mWorldRotation;
}

void TransformComponent::Deserialize(const YAML::Node& in)
{
    if (in["WorldPosition"])
        mWorldPosition = in["WorldPosition"].as<Vector3>();
    if (in["WorldRotation"])
        mWorldRotation = in["WorldRotation"].as<Quaternion>();
    mIsDirty = true;
}
}