//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include "Math/Matrix4.h"
#include "Math/Vector4.h"
#include "Render/ConstantBuffer.h"

namespace Kioto::Renderer
{
namespace EngineBuffers
{
    void Init();

    void GetTimeBufferCopy(ConstantBuffer& target);
    void GetCameraBufferCopy(ConstantBuffer& target);
};
}