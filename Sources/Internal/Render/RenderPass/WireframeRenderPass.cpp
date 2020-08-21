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
        for (auto ro : m_drawData->RenderObjects)
        {
            ro->SetExternalCB(m_passName, Renderer::SInp::Wireframe_sinp::cbCameraName, Renderer::GetMainCamera()->GetConstantBuffer().GetHandle());
            ro->SetExternalCB(m_passName, Renderer::SInp::Wireframe_sinp::cbEngineName, Renderer::EngineBuffers::GetTimeBuffer().GetHandle());

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

            commandList->PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));
        }

        commandList->PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
    }

    void WireframeRenderPass::Cleanup()
    {
    }

    void WireframeRenderPass::SetRenderTargets(CommandList* commandList, ResourceTable& resources)
    {
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
        cmd.ClearColorValue = Color::DefaultBackgroundColor;
        cmd.ClearStencil = isWireframe;
        cmd.ClearStencilValue = 0;

        commandList->PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
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