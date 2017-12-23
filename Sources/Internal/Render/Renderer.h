//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/Core.h"
#include "Core/CoreTypes.h"
#include "Render/RendererPublic.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto
{
namespace Renderer
{

class VertexLayout;

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
void AddRenderPass(const RenderPass& renderPass);
TextureHandle GetCurrentBackBufferHandle();
TextureHandle GetDepthStencilHandle();
KIOTO_API uint16 GetWidth();
KIOTO_API uint16 GetHeight();
KIOTO_API float32 GetAspect();

VertexLayoutHandle GenerateVertexLayout(const VertexLayout& layout);

}
}