//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include "Math/Matrix4.h"
#include "Math/Vector4.h"

namespace Kioto::Renderer
{
struct TimeConstantBuffer
{
    Vector4 Time; // [a_vorontsov] Time since start: (t / 20, t, t * 2, t * 3).
    Vector4 SinTime; // [a_vorontsov] Sin of time: (t / 4, t / 2, t, t * 2).
    Vector4 CosTime; // [a_vorontsov] Cos of time: (t / 4, t / 2, t, t * 2).
    Vector4 DeltaTime; // [a_vorontsov] Delta time: (dt, 1 / dt, smoothDt, 1 / smoothDt).
};

struct RenderObjectBuffer
{
    Matrix4 ToWorld;
    Matrix4 ToModel;
};

struct PassBuffer
{
    Matrix4 ViewProjection;
    Matrix4 View;
    Vector4 RTParams; // [a_vorontsov] x is the current render target width in pixels, y is the current render target height in pixels, z is (1.0 + 1.0 / width) and w is (1.0 + 1.0/height).
    Vector3 ProjParams; // [a_vorontsov] x is the camera’s near, z is the camera’s far and w is 1/FarPlane.
    float32 Pad0;
    Vector3 CamWorldPosition;
    float32 Pad1;
};

struct MaterialBuffer
{
    Vector4 Albedo;
};
}