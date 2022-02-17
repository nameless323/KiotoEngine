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

    Matrix4 mToWorld = Matrix4::Identity;
    Matrix4 mToParent = Matrix4::Identity;
    Matrix4 mToModel = Matrix4::Identity;
    bool mIsDirty = false;

    Vector3 mWorldPosition{};
    Quaternion mWorldRotation{};

    TransformComponent* mParent = nullptr;
    std::vector<TransformComponent*> mChildren;

    friend class TransformSystem;
};
REGISTER_COMPONENT(TransformComponent);

inline bool TransformComponent::GetDirty() const
{
    return mIsDirty;
}

inline const Matrix4& TransformComponent::GetToWorld() const
{
    return mToWorld;
}

inline const Matrix4& TransformComponent::GetToParent() const
{
    return mToParent;
}

inline const Matrix4& TransformComponent::GetToModel() const
{
    return mToModel;
}

inline const Vector3& TransformComponent::GetWorldPosition() const
{
    return mWorldPosition;
}

inline const Quaternion& TransformComponent::GetWorldRotation() const
{
    return mWorldRotation;
}

inline void TransformComponent::SetToWorld(const Matrix4& m)
{
    mToWorld = m;
    if (!mIsDirty)
        SetChildrenDirty();
}

inline void TransformComponent::SetToParent(const Matrix4& m)
{
    mToParent = m;
    if (!mIsDirty)
        SetChildrenDirty();
    mIsDirty = true;
}

inline void TransformComponent::SetToModel(const Matrix4& m)
{
    mToModel = m;
    if (!mIsDirty)
        SetChildrenDirty();
    mIsDirty = true;
}

inline void TransformComponent::SetParent(TransformComponent* parent)
{
    mParent = parent;
    if (!mIsDirty)
        SetChildrenDirty();
    mIsDirty = true;
}

inline void TransformComponent::SetWorldPosition(const Vector3& pos)
{
    mWorldPosition = pos;
    mIsDirty = true;
}

inline void TransformComponent::SetWorldRotation(const Quaternion& rot)
{
    mWorldRotation = rot;
    mIsDirty = true;
}

inline void TransformComponent::SetDirty()
{
    mIsDirty = true;
}

inline void TransformComponent::RemoveDirty()
{
    mIsDirty = false;
}

inline void TransformComponent::SetChildrenDirty()
{
    for (auto c : mChildren)
        c->SetDirty();
}

inline Vector3 TransformComponent::TransformPointToWorld(const Vector3& localPoint) const
{
    Vector4 worldPoint = Vector4(localPoint, 1.0f) * mToWorld;
    return { worldPoint.x, worldPoint.y, worldPoint.z };
}

inline Vector3 TransformComponent::TransformPointToModel(const Vector3& worldPoint) const
{
    Vector4 localPoint = Vector4(worldPoint, 1.0f) * mToModel;
    return { localPoint.x, localPoint.y, localPoint.z };
}

inline Vector3 TransformComponent::Up() const
{
    return { mToWorld._10, mToWorld._11, mToWorld._12 };
}

inline Vector3 TransformComponent::Right() const
{
    return { mToWorld._00, mToWorld._01, mToWorld._02 };
}

inline Vector3 TransformComponent::Fwd() const
{
    return { mToWorld._20, mToWorld._21, mToWorld._22 };
}
}