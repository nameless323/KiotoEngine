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
#include "Render/RenderObject.h"
#include "Render/RenderPacket.h"
#include "Render/Shader.h"

namespace Kioto::Renderer
{
ForwardRenderPass::ForwardRenderPass()
    : RenderPass("ForwardPass")
{
    Renderer::RegisterRenderPass(this);
    SetRenderTargetCount(1);
}

void ForwardRenderPass::CollectRenderData()
{
    SetRenderTargets();
    for (auto ro : m_renderObjects)
    {
        Material* mat = ro->GetMaterial();
        Mesh* mesh = ro->GetMesh();
        mat->BuildMaterialForPass(this);

        mat->SetValueToBuffer("ToModel", ro->GetToModel()->GetForGPU()); // [a_vorontcov] TODO: Move to render object?
        mat->SetValueToBuffer("ToWorld", ro->GetToWorld()->GetForGPU());

        RenderPacket currPacket = {};
        currPacket.Material = mat->GetHandle();
        currPacket.Shader = mat->GetShader()->GetHandle();
        currPacket.TextureSet = mat->GetShaderData().textureSet.GetHandle();
        currPacket.Mesh = mesh->GetHandle();
        currPacket.Pass = GetHandle();
        currPacket.CBSet = mat->GetShaderData().bufferSetHandle;

        PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));
    }

    PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
}

void ForwardRenderPass::Cleanup()
{
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