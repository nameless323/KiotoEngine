#include "Render/RenderPass/WireframeRenderPass.h"

#include "Core/KiotoEngine.h"
#include "Render/Camera.h"
#include "Render/Geometry/Mesh.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderObject.h"
#include "Render/RenderPacket.h"
#include "Render/RenderSettings.h"
#include "Render/Shader.h"

#include "Render/Shaders/autogen/sInp/Wireframe.h"

namespace Kioto::Renderer
{
WireframeRenderPass::WireframeRenderPass()
    : RenderPass("Wireframe")
{
    Renderer::RegisterRenderPass(this);
    SetRenderTargetCount(1);
}

void WireframeRenderPass::BuildRenderPackets(CommandList* commandList, ResourceTable& resources)
{
    SetRenderTargets(commandList, resources);
    for (auto ro : mDrawData->RenderObjects)
    {
        if (!ro->GetIsVisible())
            continue;

        ro->SetExternalCB(mPassName, Renderer::SInp::Wireframe_sinp::cbCameraName, Renderer::GetMainCamera()->GetConstantBuffer().GetHandle());
        ro->SetExternalCB(mPassName, Renderer::SInp::Wireframe_sinp::cbEngineName, Renderer::EngineBuffers::GetTimeBuffer().GetHandle());

        Material* mat = ro->GetMaterial();
        Mesh* mesh = ro->GetMesh();
        mat->BuildMaterialForPass(this);

        ro->PrepareConstantBuffers(mPassName);

        RenderPacket currPacket = {};
        currPacket.Material = mat->GetHandle();
        currPacket.Shader = mat->GetPipelineState(mPassName).Shader->GetHandle();
        currPacket.TextureSet = ro->GetTextureSet(mPassName).GetHandle();
        currPacket.Mesh = mesh->GetHandle();
        currPacket.Pass = GetHandle();
        currPacket.ConstantBufferHandles = std::move(ro->GetCBHandles(mPassName));

        commandList->PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));
    }

    commandList->PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
}

void WireframeRenderPass::Cleanup()
{
}

void WireframeRenderPass::SetRenderTargets(CommandList* commandList, ResourceTable& resources)
{
    SetRenderTargetsCommand cmd;
    cmd.SetRenderTargets(Renderer::DefaultBackBufferHandle);
    cmd.RenderTargetCount = GetRenderTargetCount();
    cmd.DepthStencil = Renderer::DefaultDepthStencilHandle;

    cmd.Viewport = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
    cmd.Scissor = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
    cmd.ClearDepth = false;
    cmd.ClearDepthValue = 0.0f;
    cmd.ClearColor = false;
    cmd.ClearColorValue = Color::DefaultBackgroundColor;
    cmd.ClearStencil = false;
    cmd.ClearStencilValue = 0;

    commandList->PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
}

bool WireframeRenderPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    const RenderSettings& settings = KiotoCore::GetRenderSettings();
    if (settings.RenderMode == RenderSettings::RenderModeOptions::Wireframe
        || settings.RenderMode == RenderSettings::RenderModeOptions::FinalAndWireframe)
        return true;
    return false;
}

}