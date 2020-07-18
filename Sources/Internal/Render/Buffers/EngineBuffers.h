#pragma once

#include <array>

#include "Core/CoreTypes.h"
#include "Math/Matrix4.h"
#include "Math/Vector4.h"

namespace Kioto::Renderer
{
class ConstantBuffer;
}

namespace Kioto::Renderer::EngineBuffers
{
struct CbEngineBuffer
{
    Vector4 Time;
    Vector4 SinTime;
    Vector4 CosTime;
    Vector4 DeltaTime;
};

struct CbCameraBuffer
{
    Matrix4 ViewProjection;
    Matrix4 View;
    Vector4 RTParams;
    Vector3 ProjParams;
    float32 Pad0;
    Vector3 CamWorldPosition;
    float32 Pad1;
};

void Init();

void GetTimeBufferCopy(ConstantBuffer& target);
void GetCameraBufferCopy(ConstantBuffer& target);

constexpr uint16 EngineBuffersSpace = 1;

constexpr uint16 TimeBufferIndex = 0;
constexpr uint16 CameraBufferIndex = 1;

constexpr uint16 EngineBuffersCount = 2;

constexpr std::array<uint16, EngineBuffersCount> BufferIndices = {{ TimeBufferIndex, CameraBufferIndex }};
}