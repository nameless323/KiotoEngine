//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

namespace Kioto::FPSCounter
{
///
/// Get last FPS.
///
uint32 GetFPS();
///
/// Get FPS averaged by 60 frames.
///
uint32 GetSmoothFPS();
///
/// Update counter.
///
void Tick(float32 dt);
}