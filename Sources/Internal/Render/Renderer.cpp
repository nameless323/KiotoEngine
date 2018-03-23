//
// Copyright (C) Alexandr Vorontsov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Renderer.h"

#include "Core/CoreHelpers.h"
#include "Render/DX12/RendererDX12.h"
#include "Systems/EventSystem/EngineEvents.h"
#include "Systems/EventSystem/EventSystem.h"

namespace Kioto::Renderer
{
namespace
{
RendererDX12* GameRenderer = nullptr; // [a_vorontsov] Not too cross-api for now.

uint16 m_height = 1024;
uint16 m_width = 768;
float32 m_aspect = 1.0f;
}

void Init(eRenderApi api, uint16 width, uint16 height)
{
    m_width = width;
    m_height = height;
    m_aspect = static_cast<float32>(m_width) / static_cast<float32>(m_height);

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
    m_width = width;
    m_height = height;
    m_aspect = static_cast<float32>(m_width) / static_cast<float32>(m_height);

    std::shared_ptr<OnMainWindowResized> e = std::make_shared<OnMainWindowResized>();
    OnMainWindowResized::Data* data = reinterpret_cast<OnMainWindowResized::Data*>(e->GetEventData());
    data->width = m_width;
    data->height = m_height;
    data->aspect = m_aspect;

    EventSystem::GlobalEventSystem.RaiseEvent(e);

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

uint16 GetWidth()
{
    return m_width;
}

uint16 GetHeight()
{
    return m_height;
}

float32 GetAspect()
{
    return m_aspect;
}

VertexLayoutHandle GenerateVertexLayout(const VertexLayout& layout)
{
    return VertexLayoutHandle(InvalidHandle);

    // [a_vorontsov] TODO;
    //return GameRenderer->GenerateVertexLayout(layout);
}

void AddRenderPass(const RenderPass& renderPass)
{
    GameRenderer->AddRenderPass(renderPass);
}

TextureHandle GetCurrentBackBufferHandle()
{
    return GameRenderer->GetCurrentBackBufferHandle();
}

TextureHandle GetDepthStencilHandle()
{
    return GameRenderer->GetDepthStencilHandle();
}

void RegisterTexture(Texture* texture)
{
    GameRenderer->RegisterTexture(texture);
}

template <typename T>
void RegisterRenderAsset(T* asset)
{
    throw "Not implemented";
}

template <>
void RegisterRenderAsset(Texture* asset)
{
    GameRenderer->RegisterTexture(asset);
}

template <>
void RegisterRenderAsset(Shader* asset)
{
    GameRenderer->RegisterShader(asset);
}

template <>
void RegisterRenderAsset(Mesh* asset)
{
    GameRenderer->RegisterMesh(asset);
}

void BuildMaterialForPass(const Material& mat, const RenderPass& pass)
{
    GameRenderer->BuildMaterialForPass(mat, pass);
}

void AllocateRenderPacketList(RenderPassHandle handle)
{
    GameRenderer->AllocateRenderPacketList(handle);
}

void AddRenderPacket(RenderPassHandle handle, RenderPacket packet)
{
    GameRenderer->AddRenderPacket(handle, packet);
}

template <>
void RegisterRenderAsset(Material* asset)
{
    GameRenderer->RegisterMaterial(asset);
}

void RegisterRenderPass(RenderPass* renderPass)
{
    GameRenderer->RegisterRenderPass(renderPass);
}

void RegisterTextureSet(TextureSet& set)
{
    GameRenderer->RegisterTextureSet(set);
}

void QueueTextureSetForUpdate(const TextureSet& set)
{
    GameRenderer->QueueTextureSetForUpdate(set);
}

template void RegisterRenderAsset<Texture>(Texture* asset);
template void RegisterRenderAsset<Shader>(Shader* asset);
template void RegisterRenderAsset<Mesh>(Mesh* asset);
template void RegisterRenderAsset<Material>(Material* asset);
}