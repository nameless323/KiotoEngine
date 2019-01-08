//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Buffers/EngineBuffers.h"

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

    static constexpr uint16 Index = 0;
    static constexpr uint16 Space = 1;
};

namespace CameraCBData
{
    std::pair<std::string, Matrix4> ViewProjection = std::make_pair("ViewProjection", Matrix4());
    std::pair<std::string, Matrix4> View = std::make_pair("View", Matrix4());
    std::pair<std::string, Vector4> RTParams = std::make_pair("RTParams", Vector4()); // [a_vorontcov] x is the current render target width in pixels, y is the current render target height in pixels, z is (1.0 + 1.0 / width) and w is (1.0 + 1.0/height).
    std::pair<std::string, Vector3> ProjParams = std::make_pair("ProjParams", Vector3()); // [a_vorontcov] x is the camera�s near, z is the camera�s far and w is 1/FarPlane.
    std::pair<std::string, float32> Pad0 = std::make_pair("Pad0", 0);
    std::pair<std::string, Vector4> CamWorldPosition = std::make_pair("CamWorldPosition", Vector4());
    std::pair<std::string, float32> Pad1 = std::make_pair("Pad1", 0);

    static constexpr uint16 Index = 1;
    static constexpr uint16 Space = 1;
};

ConstantBuffer m_cameraBuffer{ CameraCBData::Index, CameraCBData::Space };
ConstantBuffer m_timeBuffer{ TimeCBData::Index, TimeCBData::Space };
}

void EngineBuffers::Init()
{
    m_timeBuffer.Add(TimeCBData::Time.first, TimeCBData::Time.second);
    m_timeBuffer.Add(TimeCBData::SinTime.first, TimeCBData::SinTime.second);
    m_timeBuffer.Add(TimeCBData::CosTime.first, TimeCBData::CosTime.second);
    m_timeBuffer.Add(TimeCBData::DeltaTime.first, TimeCBData::DeltaTime.second);

    m_cameraBuffer.Add(CameraCBData::ViewProjection.first, CameraCBData::ViewProjection.second);
    m_cameraBuffer.Add(CameraCBData::View.first, CameraCBData::View.second);
    m_cameraBuffer.Add(CameraCBData::RTParams.first, CameraCBData::RTParams.second);
    m_cameraBuffer.Add(CameraCBData::ProjParams.first, CameraCBData::ProjParams.second);
    m_cameraBuffer.Add(CameraCBData::Pad0.first, CameraCBData::Pad0.second);
    m_cameraBuffer.Add(CameraCBData::CamWorldPosition.first, CameraCBData::CamWorldPosition.second);
    m_cameraBuffer.Add(CameraCBData::Pad1.first, CameraCBData::Pad1.second);
}

void GetTimeBufferCopy(ConstantBuffer& target)
{
    m_timeBuffer.MakeShallowCopy(target);
}

void GetCameraBufferCopy(ConstantBuffer& target)
{
    m_cameraBuffer.MakeShallowCopy(target);
}

}