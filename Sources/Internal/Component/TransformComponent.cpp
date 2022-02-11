#include "Component/TransformComponent.h"

#include "Core/Yaml/YamlParser.h"

namespace Kioto
{
Component* TransformComponent::Clone() const
{
    TransformComponent* t = new TransformComponent();
    t->m_toWorld = m_toWorld;
    t->m_toParent = m_toParent;
    t->m_toModel = m_toModel;

    t->m_children = m_children;
    t->m_parent = m_parent;
    t->m_worldPosition = m_worldPosition;
    t->m_worldRotation = m_worldRotation;

    return t;
}

void TransformComponent::Serialize(YAML::Emitter& out) const
{
    using ::operator<<;
    out << YAML::Key << "WorldPosition" << YAML::Value << m_worldPosition;
    out << YAML::Key << "WorldRotation" << YAML::Value << m_worldRotation;
}

void TransformComponent::Deserialize(const YAML::Node& in)
{
    if (in["WorldPosition"])
        m_worldPosition = in["WorldPosition"].as<Vector3>();
    if (in["WorldRotation"])
        m_worldRotation = in["WorldRotation"].as<Quaternion>();
    m_isDirty = true;
}
}