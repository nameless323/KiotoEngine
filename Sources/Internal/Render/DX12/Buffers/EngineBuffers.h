//
// Copyright (C) Alexandr Vorontsov 2016.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include <DirectXMath.h>

namespace Kioto::Renderer
{
struct TimeConstantBuffer
{
    DirectX::XMFLOAT4 Time; // [a_vorontsov] Time since start: (t / 20, t, t * 2, t * 3).
    DirectX::XMFLOAT4 SinTime; // [a_vorontsov] Sin of time: (t / 4, t / 2, t, t * 2).
    DirectX::XMFLOAT4 CosTime; // [a_vorontsov] Cos of time: (t / 4, t / 2, t, t * 2).
    DirectX::XMFLOAT4 DeltaTime; // [a_vorontsov] Delta time: (dt, 1 / dt, smoothDt, 1 / smoothDt).
};
}