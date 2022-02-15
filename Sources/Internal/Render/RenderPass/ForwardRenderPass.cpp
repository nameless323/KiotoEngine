#include "Render/RenderPass/ForwardRenderPass.h"

#include "Core/KiotoEngine.h"
#include "Render/Camera.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderObject.h"
#include "Render/RenderPacket.h"
#include "Render/RenderSettings.h"
#include "Render/Shader.h"
#include "Render/RenderGraph/ResourcesBlackboard.h"
#include "Render/RenderGraph/ResourceTable.h"
#include "Render/Geometry/Mesh.h"

#include "Render/Shaders/autogen/sInp/Fallback.h"
#include "Render/Shaders/autogen/CommonStructures.h"

#include "Component/LightComponent.h"

namespace Kioto::Renderer
{
ForwardRenderPass::ForwardRenderPass()
    : RenderPass("Forward")
{
    Renderer::RegisterRenderPass(this);
    Renderer::RegisterConstantBuffer(mLightsBuffer);

    SetRenderTargetCount(1);
}

void ForwardRenderPass::BuildRenderPackets(CommandList* commandList, ResourceTable& resources)
{
    SetRenderTargets(commandList, resources);

    for (uint32 i = 0; i < mDrawData->Lights.size(); ++i)
        mLights.Light[i] = std::move(mDrawData->Lights[i]->GetGraphicsLight());
    mLights.ShadowTransform = resources.GetPassesSharedData().ShadowTransform;
    mLightsBuffer.Set(mLights);

    Texture* shadowMap = resources.GetResource("ShadowMap");

    for (auto ro : mDrawData->RenderObjects)
    {
        if (!ro->GetIsVisible())
            continue;

        ro->SetExternalCB(mPassName, Renderer::SInp::Fallback_sinp::cbCameraName, Renderer::GetMainCamera()->GetConstantBuffer().GetHandle());
        ro->SetExternalCB(mPassName, Renderer::SInp::Fallback_sinp::cbEngineName, Renderer::EngineBuffers::GetTimeBuffer().GetHandle());
        ro->SetExternalCB(mPassName, Renderer::SInp::Fallback_sinp::lightsName, mLightsBuffer.GetHandle());
        ro->SetConstant(mPassName, "LIGHTS_COUNT", static_cast<uint32>(mDrawData->Lights.size()));
        Material* mat = ro->GetMaterial();
        Mesh* mesh = ro->GetMesh();
        mat->BuildMaterialForPass(this);

        ro->PrepareConstantBuffers(mPassName);

        ro->SetTexture("ShadowTexture", shadowMap, mPassName);

        RenderPacket currPacket = {};
        currPacket.Material = mat->GetHandle();
        currPacket.Shader = mat->GetPipelineState(mPassName).Shader->GetHandle();
        currPacket.TextureSet = ro->GetTextureSet(mPassName).GetHandle();
        currPacket.Mesh = mesh->GetHandle();
        currPacket.Pass = GetHandle();
        currPacket.ConstantBufferHandles = std::move(ro->GetCBHandles(mPassName));
        currPacket.UniformConstants = std::move(ro->GetConstants(mPassName));

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

bool ForwardRenderPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    const RenderSettings& settings = KiotoCore::GetRenderSettings();

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

    resources.ScheduleRead("ShadowMap");

    if (settings.RenderMode == RenderSettings::RenderModeOptions::Final
        || settings.RenderMode == RenderSettings::RenderModeOptions::FinalAndWireframe)
        return true;
    return false;
}

}