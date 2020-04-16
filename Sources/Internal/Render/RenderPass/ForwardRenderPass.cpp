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
    : RenderPass("Forward")
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

        ro->SetValueToBuffer("ToModel", ro->GetToModel()->GetForGPU(), m_passName);// [a_vorontcov] TODO: Move to render object?
        ro->SetValueToBuffer("ToWorld", ro->GetToWorld()->GetForGPU(), m_passName);

        RenderPacket currPacket = {};
        currPacket.Material = mat->GetHandle();
        currPacket.Shader = mat->GetPipelineState(m_passName).Shader->GetHandle();
        currPacket.TextureSet = ro->GetTextureSet(m_passName).GetHandle();
        currPacket.Mesh = mesh->GetHandle();
        currPacket.Pass = GetHandle();
        currPacket.CBSet = ro->GetBufferLayout(m_passName).bufferSetHandle;

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