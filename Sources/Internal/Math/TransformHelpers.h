#pragma once

#include "Core/CoreTypes.h"

#include "Component/TransformComponent.h"
#include "Math/Vector3.h"

namespace Kioto::Math::TransformHelpers
{
/// Returns new position after rotation of source transform around targetPos but angles xAngle and yAngle. DOES NOT MODIFY TRANSFORM ITSELF.
inline Vector3 RotateAround(const TransformComponent& source, const Vector3& targetPos, float32 xAngle, float32 yAngle)
{
    Vector3 transfVector = source.GetWorldPosition() - targetPos;
    Quaternion xRot(source.Up(), xAngle);
    Quaternion yRot(source.Right(), yAngle);
    Quaternion rot = yRot * xRot;
    transfVector = rot.RotateVector(transfVector);
    transfVector += targetPos;
    return transfVector;
}
}
