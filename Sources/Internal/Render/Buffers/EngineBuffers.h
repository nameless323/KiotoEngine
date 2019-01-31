//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include "Math/Matrix4.h"
#include "Math/Vector4.h"
#include "Render/ConstantBuffer.h"
#include <utility>

namespace Kioto::Renderer
{
namespace EngineBuffers
{
constexpr uint16 EngineBuffersSpace = 1;

constexpr uint16 TimeBufferIndex = 0;
constexpr uint16 CameraBufferIndex = 1;

constexpr uint16 EngineBuffersCount = 2;

constexpr std::array<uint16, EngineBuffersCount> BufferIndices = {{ TimeBufferIndex, CameraBufferIndex }};

void Init();

void GetTimeBufferCopy(ConstantBuffer& target);
void GetCameraBufferCopy(ConstantBuffer& target);
};
}