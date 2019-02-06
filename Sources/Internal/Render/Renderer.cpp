//
// Copyright (C) Aleksandr Vorontcov. 2017
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/Renderer.h"

#include "Core/CoreHelpers.h"
#include "Core/Timer/GlobalTimer.h"
#include "Render/DX12/RendererDX12.h"
#include "Systems/EventSystem/EngineEvents.h"
#include "Systems/EventSystem/EventSystem.h"
#include "Render/DX12/PixProfiler.h"
#include "Render/Buffers/EngineBuffers.h"

namespace Kioto::Renderer
{
namespace
{
RendererDX12* GameRenderer = nullptr; // [a_vorontcov] Not too cross-api for now.

uint16 m_height = 1024;
uint16 m_width = 768;
float32 m_aspect = 1.0f;

Camera* m_mainCamera;

ConstantBuffer m_timeBuffer; // [a_vorontcov] Find a better place.

void UpdateTimeBuffer()
{
    float32 timeFromStart = static_cast<float32>(GlobalTimer::GetTimeFromStart());
    m_timeBuffer.Set("Time", Vector4(timeFromStart / 20.0f, timeFromStart, timeFromStart * 2, timeFromStart * 3), false);
    m_timeBuffer.Set("SinTime", Vector4(sin(timeFromStart / 4.0f), sin(timeFromStart / 2.0f), sin(timeFromStart), sin(timeFromStart * 2.0f)), false);
    m_timeBuffer.Set("CosTime", Vector4(cos(timeFromStart / 4.0f), cos(timeFromStart / 2.0f), cos(timeFromStart), cos(timeFromStart * 2.0f)), false);
    float32 dt = static_cast<float32>(GlobalTimer::GetDeltaTime());
    float32 smoothDt = static_cast<float32>(GlobalTimer::GetSmoothDt());
    m_timeBuffer.Set("DeltaTime", Vector4(dt, 1.0f / dt, smoothDt, 1.0f / smoothDt));
}
}

void Init(eRenderApi api, uint16 width, uint16 height)
{
    m_width = width;
    m_height = height;
    m_aspect = static_cast<float32>(m_width) / static_cast<float32>(m_height);

    GameRenderer = new RendererDX12();
    if (api == eRenderApi::DirectX12)
        GameRenderer->Init(width, height);

    EngineBuffers::GetTimeBufferCopy(m_timeBuffer);
    m_timeBuffer.ComposeBufferData();
    GameRenderer->RegisterConstantBuffer(m_timeBuffer);
    GameRenderer->SetTimeBuffer(m_timeBuffer.GetHandle());
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

void Update(float32 dt) // [a_vorontcov] TODO: set frame command buffers here.
{
    UpdateTimeBuffer();
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

    // [a_vorontcov] TODO;
    //return GameRenderer->GenerateVertexLayout(layout);
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

void BuildMaterialForPass(Material& mat, const RenderPass* pass)
{
    GameRenderer->BuildMaterialForPass(mat, pass);
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

void SetMainCamera(Camera* camera)
{
    m_mainCamera = camera;
}

Camera* GetMainCamera()
{
    return m_mainCamera; // by reference? depends, think bout it
}

void SubmitRenderCommands(const std::vector<RenderCommand>& commandList)
{
    GameRenderer->SubmitRenderCommands(commandList);
}

void QueueConstantBufferForUpdate(ConstantBuffer& buffer)
{
    GameRenderer->QueueConstantBufferForUpdate(buffer);
}

void RegisterConstantBuffer(ConstantBuffer& buffer)
{
    GameRenderer->RegisterConstantBuffer(buffer);
}

template void RegisterRenderAsset<Texture>(Texture* asset);
template void RegisterRenderAsset<Shader>(Shader* asset);
template void RegisterRenderAsset<Mesh>(Mesh* asset);
template void RegisterRenderAsset<Material>(Material* asset);
}