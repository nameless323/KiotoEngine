#include "stdafx.h"

#include "Render/RenderPass/WireframeRenderPass.h"

#include "Core/KiotoEngine.h"
#include "Render/Camera.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderObject.h"
#include "Render/RenderPacket.h"
#include "Render/RenderOptions.h"
#include "Render/Shader.h"

namespace Kioto::Renderer
{
    WireframeRenderPass::WireframeRenderPass()
        : RenderPass("Wireframe")
    {
        Renderer::RegisterRenderPass(this);
        SetRenderTargetCount(1);
    }

    void WireframeRenderPass::BuildRenderPackets(CommandList* commandList, ResourcesBlackboard& resources)
    {
        SetRenderTargets(commandList);
        for (auto ro : m_renderObjects)
        {
            Material* mat = ro->GetMaterial();
            Mesh* mesh = ro->GetMesh();
            mat->BuildMaterialForPass(this);

            ro->SetValueToBuffer("ToModel", ro->GetToModel()->GetForGPU(), m_passName);
            ro->SetValueToBuffer("ToWorld", ro->GetToWorld()->GetForGPU(), m_passName);

            RenderPacket currPacket = {};
            currPacket.Material = mat->GetHandle();
            currPacket.Shader = mat->GetPipelineState(m_passName).Shader->GetHandle();
            currPacket.TextureSet = ro->GetTextureSet(m_passName).GetHandle();
            currPacket.Mesh = mesh->GetHandle();
            currPacket.Pass = GetHandle();
            currPacket.CBSet = ro->GetBufferLayout(m_passName).bufferSetHandle;

            commandList->PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));
        }

        commandList->PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
    }

    void WireframeRenderPass::Cleanup()
    {
    }

    void WireframeRenderPass::SetRenderTargets(CommandList* commandList)
    {
        SetPassConstantBuffers(commandList);
        SetCameraConstantBuffers(commandList);
        bool isWireframe = KiotoCore::GetRenderSettings().RenderMode == RenderOptions::RenderModeOptions::Wireframe;
        SetRenderTargetsCommand cmd;
        cmd.SetRenderTargets(Renderer::DefaultBackBufferHandle);
        cmd.RenderTargetCount = GetRenderTargetCount();
        cmd.DepthStencil = Renderer::DefaultDepthStencilHandle;

        cmd.Viewport = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
        cmd.Scissor = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
        cmd.ClearDepth = isWireframe;
        cmd.ClearDepthValue = 0.0f;
        cmd.ClearColor = isWireframe;
        cmd.ClearStencil = isWireframe;
        cmd.ClearStencilValue = 0;

        commandList->PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
    }

    void WireframeRenderPass::SetPassConstantBuffers(CommandList* commandList)
    {

    }

    void WireframeRenderPass::SetCameraConstantBuffers(CommandList* commandList)
    {
        commandList->PushCommand(RenderCommandHelpers::CreateConstantBufferCommand(Renderer::GetMainCamera()->GetConstantBuffer(), this));
    }

    bool WireframeRenderPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
    {
        const RenderOptions& settings = KiotoCore::GetRenderSettings();
        if (settings.RenderMode == RenderOptions::RenderModeOptions::Wireframe
            || settings.RenderMode == RenderOptions::RenderModeOptions::FinalAndWireframe)
            return true;
        return false;
    }

}