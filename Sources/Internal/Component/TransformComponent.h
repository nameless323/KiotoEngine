#pragma once

#include <vector>

#include "Core/CoreTypes.h"
#include "Core/ECS/Component.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

namespace Kioto
{
class TransformComponent : public Component
{
    DECLARE_COMPONENT(TransformComponent);

public:
    KIOTO_API TransformComponent() = default;
    KIOTO_API ~TransformComponent() = default;

    bool GetDirty() const;
    const Matrix4& GetToWorld() const;
    const Matrix4& GetToParent() const;
    const Matrix4& GetToModel() const;
    const Vector3& GetWorldPosition() const;
    const Quaternion& GetWorldRotation() const;

    Vector3 TransformPointToWorld(const Vector3& localPoint) const;
    Vector3 TransformPointToModel(const Vector3& worldPoint) const;

    void SetToWorld(const Matrix4& m);
    void SetToParent(const Matrix4& m);
    void SetToModel(const Matrix4& m);
    void SetParent(TransformComponent* parent);
    void SetWorldPosition(const Vector3& pos);
    void SetWorldRotation(const Quaternion& rot);

    Vector3 Up() const;
    Vector3 Right() const;
    Vector3 Fwd() const;

    Component* Clone() const override;

    void Serialize(YAML::Emitter& out) const override;
    void Deserialize(const YAML::Node& in) override;

private:
    void SetDirty();
    void RemoveDirty();
    void SetChildrenDirty();

    Matrix4 m_toWorld = Matrix4::Identity;
    Matrix4 m_toParent = Matrix4::Identity;
    Matrix4 m_toModel = Matrix4::Identity;
    bool m_isDirty = false;

    Vector3 m_worldPosition{};
    Quaternion m_worldRotation{}; // [a_vorontcov] TODO: quaternion.

    TransformComponent* m_parent = nullptr;
    std::vector<TransformComponent*> m_children;

    friend class TransformSystem;
};
REGISTER_COMPONENT(TransformComponent);

inline bool TransformComponent::GetDirty() const
{
    return m_isDirty;
}

inline const Matrix4& TransformComponent::GetToWorld() const
{
    return m_toWorld;
}

inline const Matrix4& TransformComponent::GetToParent() const
{
    return m_toParent;
}

inline const Matrix4& TransformComponent::GetToModel() const
{
    return m_toModel;
}

inline const Vector3& TransformComponent::GetWorldPosition() const
{
    return m_worldPosition;
}

inline const Quaternion& TransformComponent::GetWorldRotation() const
{
    return m_worldRotation;
}

inline void TransformComponent::SetToWorld(const Matrix4& m)
{
    m_toWorld = m;
    if (!m_isDirty)
        SetChildrenDirty();
}

inline void TransformComponent::SetToParent(const Matrix4& m)
{
    m_toParent = m;
    if (!m_isDirty)
        SetChildrenDirty();
    m_isDirty = true;
}

inline void TransformComponent::SetToModel(const Matrix4& m)
{
    m_toModel = m;
    if (!m_isDirty)
        SetChildrenDirty();
    m_isDirty = true;
}

inline void TransformComponent::SetParent(TransformComponent* parent)
{
    m_parent = parent;
    if (!m_isDirty)
        SetChildrenDirty();
    m_isDirty = true;
}

inline void TransformComponent::SetWorldPosition(const Vector3& pos)
{
    m_worldPosition = pos;
    m_isDirty = true;
}

inline void TransformComponent::SetWorldRotation(const Quaternion& rot)
{
    m_worldRotation = rot;
    m_isDirty = true;
}

inline void TransformComponent::SetDirty()
{
    m_isDirty = true;
}

inline void TransformComponent::RemoveDirty()
{
    m_isDirty = false;
}

inline void TransformComponent::SetChildrenDirty()
{
    for (auto c : m_children)
        c->SetDirty();
}

inline Vector3 TransformComponent::TransformPointToWorld(const Vector3& localPoint) const
{
    Vector4 worldPoint = Vector4(localPoint, 1.0f) * m_toWorld;
    return { worldPoint.x, worldPoint.y, worldPoint.z };
}

inline Vector3 TransformComponent::TransformPointToModel(const Vector3& worldPoint) const
{
    Vector4 localPoint = Vector4(worldPoint, 1.0f) * m_toModel;
    return { localPoint.x, localPoint.y, localPoint.z };
}

inline Vector3 TransformComponent::Up() const
{
    return { m_toWorld._10, m_toWorld._11, m_toWorld._12 };
}

inline Vector3 TransformComponent::Right() const
{
    return { m_toWorld._00, m_toWorld._01, m_toWorld._02 };
}

inline Vector3 TransformComponent::Fwd() const
{
    return { m_toWorld._20, m_toWorld._21, m_toWorld._22 };
}
}