//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Buffers/EngineBuffers.h"

namespace Kioto::Renderer::EngineBuffers
{

namespace
{
namespace TimeCBData
{
    std::pair<std::string, Vector4> Time = std::make_pair("Time", Vector4()); // [a_vorontcov] Time since start: (t / 20, t, t * 2, t * 3).
    std::pair<std::string, Vector4> SinTime = std::make_pair("SinTime", Vector4()); // [a_vorontcov] Sin of time: (t / 4, t / 2, t, t * 2).
    std::pair<std::string, Vector4> CosTime = std::make_pair("CosTime", Vector4()); // [a_vorontcov] Cos of time: (t / 4, t / 2, t, t * 2).
    std::pair<std::string, Vector4> DeltaTime = std::make_pair("DeltaTime", Vector4()); // [a_vorontcov] Delta time: (dt, 1 / dt, smoothDt, 1 / smoothDt).
};

namespace CameraCBData
{
    std::pair<std::string, Matrix4> ViewProjection = std::make_pair("ViewProjection", Matrix4());
    std::pair<std::string, Matrix4> View = std::make_pair("View", Matrix4());
    std::pair<std::string, Vector4> RTParams = std::make_pair("RTParams", Vector4()); // [a_vorontcov] x is the current render target width in pixels, y is the current render target height in pixels, z is (1.0 + 1.0 / width) and w is (1.0 + 1.0/height).
    std::pair<std::string, Vector3> ProjParams = std::make_pair("ProjParams", Vector3()); // [a_vorontcov] x is the camera’s near, z is the camera’s far and w is 1/FarPlane.
    std::pair<std::string, float32> Pad0 = std::make_pair("Pad0", 0.0f);
    std::pair<std::string, Vector4> CamWorldPosition = std::make_pair("CamWorldPosition", Vector4());
    std::pair<std::string, float32> Pad1 = std::make_pair("Pad1", 0.0f);
};

ConstantBuffer m_cameraBuffer{ CameraBufferIndex, EngineBuffersSpace };
ConstantBuffer m_timeBuffer{ TimeBufferIndex, EngineBuffersSpace };
}

void EngineBuffers::Init()
{
    m_timeBuffer.Add(TimeCBData::Time.first, TimeCBData::Time.second, false);
    m_timeBuffer.Add(TimeCBData::SinTime.first, TimeCBData::SinTime.second, false);
    m_timeBuffer.Add(TimeCBData::CosTime.first, TimeCBData::CosTime.second, false);
    m_timeBuffer.Add(TimeCBData::DeltaTime.first, TimeCBData::DeltaTime.second, false);

    m_cameraBuffer.Add(CameraCBData::ViewProjection.first, CameraCBData::ViewProjection.second, false);
    m_cameraBuffer.Add(CameraCBData::View.first, CameraCBData::View.second, false);
    m_cameraBuffer.Add(CameraCBData::RTParams.first, CameraCBData::RTParams.second, false);
    m_cameraBuffer.Add(CameraCBData::ProjParams.first, CameraCBData::ProjParams.second, false);
    m_cameraBuffer.Add(CameraCBData::Pad0.first, CameraCBData::Pad0.second, false);
    m_cameraBuffer.Add(CameraCBData::CamWorldPosition.first, CameraCBData::CamWorldPosition.second, false);
    m_cameraBuffer.Add(CameraCBData::Pad1.first, CameraCBData::Pad1.second, false);
}

void GetTimeBufferCopy(ConstantBuffer& target)
{
    m_timeBuffer.MakeShallowCopy(target, false);
    target.ComposeBufferData();
}

void GetCameraBufferCopy(ConstantBuffer& target)
{
    m_cameraBuffer.MakeShallowCopy(target, false);
    target.ComposeBufferData();
}

}