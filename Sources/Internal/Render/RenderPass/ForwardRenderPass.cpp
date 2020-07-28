#include "stdafx.h"

#include "Render/RenderPass/ForwardRenderPass.h"

#include "Core/KiotoEngine.h"
#include "Render/Camera.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderObject.h"
#include "Render/RenderPacket.h"
#include "Render/RenderOptions.h"
#include "Render/Shader.h"
#include "Render/RenderGraph/ResourcesBlackboard.h"
#include "Render/RenderGraph/ResourceTable.h"

namespace Kioto::Renderer
{
ForwardRenderPass::ForwardRenderPass()
    : RenderPass("Forward")
{
    Renderer::RegisterRenderPass(this);
    SetRenderTargetCount(1);
}

void ForwardRenderPass::BuildRenderPackets(CommandList* commandList, ResourceTable& resources)
{
    SetPassConstantBuffers(commandList);
    SetCameraConstantBuffers(commandList);
    SetRenderTargets(commandList, resources);
    for (auto ro : m_renderObjects)
    {
        ro->SetExternalCB(m_passName, "cbCameraBuffer", Renderer::GetMainCamera()->GetConstantBuffer().GetHandle());
        ro->SetExternalCB(m_passName, "cbEngineBuffer", Renderer::EngineBuffers::GetTimeBuffer().GetHandle());
        ro->SetConstant(m_passName, "LIGHTS_COUNT", 4);
        Material* mat = ro->GetMaterial();
        Mesh* mesh = ro->GetMesh();
        mat->BuildMaterialForPass(this);

        ro->PrepareConstantBuffers(m_passName);

        RenderPacket currPacket = {};
        currPacket.Material = mat->GetHandle();
        currPacket.Shader = mat->GetPipelineState(m_passName).Shader->GetHandle();
        currPacket.TextureSet = ro->GetTextureSet(m_passName).GetHandle();
        currPacket.Mesh = mesh->GetHandle();
        currPacket.Pass = GetHandle();
        currPacket.ConstantBufferHandles = std::move(ro->GetCBHandles(m_passName));
        currPacket.UniformConstants = std::move(ro->GetConstants(m_passName));

        commandList->PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));
    }

    commandList->PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
}

void ForwardRenderPass::Cleanup()
{
}

void ForwardRenderPass::SetRenderTargets(CommandList* commandList, ResourceTable& resources)
{
    SetRenderTargetsCommand cmd;
    Texture* rtTex = resources.GetResource("FwdTargetTexture");
    cmd.SetRenderTargets(rtTex->GetHandle());
    //cmd.SetRenderTargets(Renderer::DefaultBackBufferHandle);
    cmd.RenderTargetCount = GetRenderTargetCount();
    cmd.DepthStencil = Renderer::DefaultDepthStencilHandle;

    cmd.Viewport = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
    cmd.Scissor = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
    cmd.ClearDepth = true;
    cmd.ClearDepthValue = 0.0f;
    cmd.ClearColor = true;
    cmd.ClearColorValue = Color::DefaultBackgroundColor;
    cmd.ClearStencil = true;
    cmd.ClearStencilValue = 0;

    commandList->PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
}

void ForwardRenderPass::SetPassConstantBuffers(CommandList* commandList)
{

}

void ForwardRenderPass::SetCameraConstantBuffers(CommandList* commandList)
{
}

bool ForwardRenderPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    const RenderOptions& settings = KiotoCore::GetRenderSettings();

    TextureDescriptor desc;
    desc.Dimension = eResourceDim::Texture2D;
    desc.Format = eResourceFormat::Format_R8G8B8A8_UNORM;
    desc.Flags = eResourceFlags::AllowRenderTarget;
    desc.Width = Renderer::GetWidth();
    desc.Height = Renderer::GetHeight();
    desc.InitialState = eResourceState::Common;
    desc.FastClear = true;
    desc.FastClearValue = Color::DefaultBackgroundColor;
    desc.Name = "FwdTargetTexture";

    resources.NewTexture("FwdTargetTexture", std::move(desc));
    resources.ScheduleWrite("FwdTargetTexture");

    if (settings.RenderMode == RenderOptions::RenderModeOptions::Final
        || settings.RenderMode == RenderOptions::RenderModeOptions::FinalAndWireframe)
        return true;
    return false;
}

}