//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Core/Core.h"
#include "Core/CoreTypes.h"
#include "Render/RendererPublic.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/DX12/RenderPacket.h"
#include "Render/Camera.h"

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

// Push pass?
// Pop pass?
void SetRenderPass(const RenderPass& renderPass);
void SetMainCamera(const Camera& camera); // Set camera command buffers.
Camera GetMainCamera();

void SubmitRenderCommands(const std::list<RenderCommand>& commandList);

void AllocateRenderPacketList(RenderPassHandle handle);
void AddRenderPacket(RenderPassHandle handle, RenderPacket packet);

void QueueTextureSetForUpdate(const TextureSet& set);

template <typename T>
void RegisterRenderAsset(T* asset);
void RegisterRenderPass(RenderPass* renderPass); // [a_vorontcov] TODO: allocate passes and all packet lists in renderer and give them by request.
void RegisterTextureSet(TextureSet& set);
void RegisterConstantBuffer(ConstantBuffer& buffer);
TextureHandle GetCurrentBackBufferHandle();
TextureHandle GetDepthStencilHandle();
KIOTO_API uint16 GetWidth();
KIOTO_API uint16 GetHeight();
KIOTO_API float32 GetAspect();

VertexLayoutHandle GenerateVertexLayout(const VertexLayout& layout);
void BuildMaterialForPass(Material& mat, const RenderPass* pass);

}
}