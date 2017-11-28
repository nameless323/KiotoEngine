//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"
#include "Core/ECS/Component.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include <vector>

namespace Kioto
{
class TransformComponent : public Component
{
    DECLARE_COMPONENT(TransformComponent);

public:
    TransformComponent() = default;
    ~TransformComponent() = default;

    bool GetTransformDirty() const;
    const Matrix4& GetToWorld() const;
    const Matrix4& GetToParent() const;
    const Matrix4& GetToModel() const;
    const Vector3& GetWorldPosition() const;
    const Matrix4& GetWorldRotation() const;

    void SetToWorld(const Matrix4& m);
    void SetToParent(const Matrix4& m);
    void SetToModel(const Matrix4& m);
    void SetParent(TransformComponent* parent);
    void SetWorldPosition(const Vector3& pos);
    void SetWorldRotation(const Matrix4& rot);

    Component* Clone() const override;

private:
    Matrix4 m_toWorld = Matrix4::Identity;
    Matrix4 m_toParent = Matrix4::Identity;
    Matrix4 m_toModel = Matrix4::Identity;
    bool m_isDirty = false;
    void SetDirty();
    void RemoveDirty();
    void SetChildrenDirty();

    Vector3 m_worldPosition{};
    Matrix4 m_worldRotation{}; // [a_vorontsov] TODO: quaternion.

    TransformComponent* m_parent = nullptr;
    std::vector<TransformComponent*> m_children;
};

inline bool TransformComponent::GetTransformDirty() const
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

inline const Matrix4& TransformComponent::GetWorldRotation() const
{
    return m_worldRotation;
}

inline void TransformComponent::SetToWorld(const Matrix4& m)
{
    m_toWorld = m;
    if (!m_isDirty)
        SetChildrenDirty();
    m_isDirty = true;
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

inline void TransformComponent::SetWorldRotation(const Matrix4& rot)
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

void TransformComponent::SetChildrenDirty()
{
    for (auto c : m_children)
        c->SetDirty();
}

}