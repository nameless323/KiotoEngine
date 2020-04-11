#pragma once

#include <vector>

#include "Core/Core.h"
#include "Core/CoreTypes.h"
#include "Render/RendererPublic.h"
#include "Render/RenderCommand.h"

namespace Kioto::Renderer
{
class VertexLayout;
class Camera;
class RenderObject;

enum class eRenderApi
{
    DirectX12
};

void Init(eRenderApi api, uint16 width, uint16 height);
void Shutdown();
void Resize(uint16 width, uint16 height, bool minimized);
void StartFrame();
void Update(float32 dt);
void Present();
void ChangeFullScreenMode(bool fullScreen);

void SetMainCamera(Camera* camera); // Set camera command buffers.
Camera* GetMainCamera();

void SubmitRenderCommands(const std::vector<RenderCommand>& commandList);

void QueueTextureSetForUpdate(const TextureSet& set);
void QueueConstantBufferForUpdate(ConstantBuffer& buffer);

template <typename T>
void RegisterRenderAsset(T* asset);
void RegisterRenderPass(RenderPass* renderPass);
void RegisterTextureSet(TextureSet& set);
void RegisterConstantBuffer(ConstantBuffer& buffer);
void RegisterRenderObject(RenderObject& renderObject);
TextureHandle GetCurrentBackBufferHandle();
TextureHandle GetDepthStencilHandle();
KIOTO_API uint16 GetWidth();
KIOTO_API uint16 GetHeight();
KIOTO_API float32 GetAspect();

VertexLayoutHandle GenerateVertexLayout(const VertexLayout& layout);
void BuildMaterialForPass(Material& mat, const RenderPass* pass);
}