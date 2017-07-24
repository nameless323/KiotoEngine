//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/CoreTypes.h"

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
void ChangeFullscreenMode(bool fullScreen);

}
}