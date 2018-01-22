//
// Copyright (C) Alexandr Vorontsov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/DX12/Buffers/EngineBuffers.h"

namespace Kioto::Renderer
{

void EngineBuffers::Init()
{
    TimeCB.Add("Time", Vector4()); // [a_vorontsov] Time since start: (t / 20, t, t * 2, t * 3).
    TimeCB.Add("SinTime", Vector4()); // [a_vorontsov] Sin of time: (t / 4, t / 2, t, t * 2).
    TimeCB.Add("CosTime", Vector4()); // [a_vorontsov] Cos of time: (t / 4, t / 2, t, t * 2).
    TimeCB.Add("DeltaTime", Vector4()); // [a_vorontsov] Delta time: (dt, 1 / dt, smoothDt, 1 / smoothDt).
    TimeCB.ComposeBufferData();

    RenderObjectCB.Add("ToWorld", Matrix4());
    RenderObjectCB.Add("ToModel", Matrix4());
    RenderObjectCB.ComposeBufferData();

    PassCB.Add("ViewProjection", Matrix4());
    PassCB.Add("View", Matrix4());
    PassCB.Add("RTParams", Vector4());
    PassCB.Add("ProjParams", Vector3());
    PassCB.Add("Pad0", 0.0f);
    PassCB.Add("CamWorldPosition", Vector3());
    PassCB.Add("Pad1", 0.0f);
    PassCB.ComposeBufferData();
}

}