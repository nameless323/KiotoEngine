//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#include "stdafx.h"

#include "Render/RenderPass/ForwardRenderPass.h"

#include "Render/Camera.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderPacket.h"
#include "Render/Shader.h"

namespace Kioto::Renderer
{
ForwardRenderPass::ForwardRenderPass()
    : RenderPass("ForwardPass")
{
    m_material = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Material>(m_matPath);
    m_mesh = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Mesh>(m_meshPath);

    Renderer::RegisterRenderPass(this);
    SetRenderTargetCount(1);

    m_material->BuildMaterialForPass(this);
}

void ForwardRenderPass::CollectRenderData()
{
    SetRenderTargets();
    UpdateBuffer();

    RenderPacket currPacket;
    currPacket.Material = m_material->GetHandle();
    currPacket.Shader = m_material->GetShader()->GetHandle();
    currPacket.TextureSet = m_material->GetShaderData().textureSet.GetHandle();
    currPacket.Mesh = m_mesh->GetHandle();
    currPacket.Pass = GetHandle();
    currPacket.CBSet = m_material->GetShaderData().bufferSetHandle;

    PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));

    PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
}

void ForwardRenderPass::Cleanup()
{
}

void ForwardRenderPass::UpdateBuffer()
{
    Matrix4 toWorld = Matrix4::Identity; //Matrix4::BuildRotation(Vector3(1.0f, 1.0f, 0.0f).Normalize(), angle);

    toWorld.SetTranslation({ 1.0f, 0.0f, 2.0f });

    Matrix4 toModel;
    toWorld.Inversed(toModel);

    m_material->SetValueToBuffer("ToModel", toModel.GetForGPU());
    m_material->SetValueToBuffer("ToWorld", toWorld.GetForGPU());
}

void ForwardRenderPass::SetRenderTargets()
{
    SetRenderTargetsCommand cmd;
    cmd.SetRenderTargets(Renderer::DefaultBackBufferHandle);
    cmd.RenderTargetCount = GetRenderTargetCount();
    cmd.DepthStencil = Renderer::DefaultDepthStencilHandle;

    cmd.Viewport = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
    cmd.Scissor = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
    cmd.ClearDepth = true;
    cmd.ClearDepthValue = 0.0f;
    cmd.ClearColor = true;
    cmd.ClearStencil = true;
    cmd.ClearStencilValue = 0;

    PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
}

void ForwardRenderPass::SetPassConstantBuffers()
{

}

void ForwardRenderPass::SetCameraConstantBuffers()
{
    PushCommand(RenderCommandHelpers::CreateConstantBufferCommand(Renderer::GetMainCamera()->GetConstantBuffer(), this));
}
}