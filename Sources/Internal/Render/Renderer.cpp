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
RendererDX12* renderer = nullptr; // [a_vorontsov] Not too cross-api for now.
}

void Init(eRenderApi api, uint16 width, uint16 height)
{
    renderer = new RendererDX12();
    if (api == eRenderApi::DirectX12)
        renderer->Init(width, height);
}

void Shutdown()
{
    renderer->Shutdown();
    SafeDelete(&renderer);
}

void Resize(uint16 width, uint16 height, bool minimized)
{
    renderer->Resize(width, height);
}

void ChangeFullScreenMode(bool fullScreen)
{
    renderer->ChangeFullScreenMode(fullScreen);
}

void Update(float32 dt) // [a_vorontsov] TODO: set frame command buffers here.
{

}

void Present()
{
    renderer->Present();
}

}