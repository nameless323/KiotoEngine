//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

#include "Core/Core.h"

namespace Kioto
{
namespace Renderer
{

enum class eRenderApi
{
    DirectX12
};

void Init(eRenderApi api, uint16 width, uint16 height);
void Shutdown();
void Resize(uint16 width, uint16 height, bool minimized);
void Update(float32 dt);
void Present();
void ChangeFullScreenMode(bool fullScreen);
KIOTO_API uint16 GetWidth();
KIOTO_API uint16 GetHeight();
KIOTO_API float32 GetAspect();

}
}