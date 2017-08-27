//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Renderer.h"

#include "Core/CoreHelpers.h"
#include "Render/DX12/RendererDX12.h"

namespace Kioto::Renderer
{
namespace
{
RendererDX12* GameRenderer = nullptr; // [a_vorontsov] Not too cross-api for now.
}

void Init(eRenderApi api, uint16 width, uint16 height)
{
    GameRenderer = new RendererDX12();
    if (api == eRenderApi::DirectX12)
        GameRenderer->Init(width, height);
}

void Shutdown()
{
    GameRenderer->Shutdown();
    SafeDelete(GameRenderer);
}

void Resize(uint16 width, uint16 height, bool minimized)
{
    GameRenderer->Resize(width, height);
}

void ChangeFullScreenMode(bool fullScreen)
{
    GameRenderer->ChangeFullScreenMode(fullScreen);
}

void Update(float32 dt) // [a_vorontsov] TODO: set frame command buffers here.
{
    GameRenderer->Update(dt);
}

void Present()
{
    GameRenderer->Present();
}

}