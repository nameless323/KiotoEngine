#include "stdafx.h"

#include "Render/RenderPass/ShadowMapRenderPass.h"

#include "Core/KiotoEngine.h"
#include "Render/Camera.h"
#include "Render/Geometry/GeometryGenerator.h"
#include "Render/Lighting/Light.h"
#include "Render/Material.h"
#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderObject.h"
#include "Render/RenderPacket.h"
#include "Render/RenderSettings.h"
#include "Render/Shader.h"
#include "Render/RenderGraph/ResourcesBlackboard.h"
#include "Render/RenderGraph/ResourceTable.h"

#include "Render/Shaders/autogen/sInp/ShadowMap.h"

namespace Kioto::Renderer
{
ShadowMapRenderPass::ShadowMapRenderPass()
    : RenderPass("ShadowMap")
{
    Renderer::RegisterRenderPass(this);

    SetRenderTargetCount(0);
    m_debugQuad = GeometryGenerator::GetFullscreenQuad();
}

bool ShadowMapRenderPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    TextureDescriptor desc;
    desc.Dimension = eResourceDim::Texture2D;
    desc.Format = eResourceFormat::Format_D24_UNORM_S8_UINT;
    desc.Flags = eResourceFlags::AllowDepthStencil;
    desc.Width = m_shadowmapSize;
    desc.Height = m_shadowmapSize;
    desc.InitialState = eResourceState::Common;
    desc.FastClear = true;
    desc.FastClearValue = Vector2(1.0f, 0.0f);
    desc.Name = "ShadowMap";

    resources.NewTexture("ShadowMap", std::move(desc));
    resources.ScheduleWriteDS("ShadowMap");

    const RenderSettings& settings = KiotoCore::GetRenderSettings();
    if (settings.RenderMode == RenderSettings::RenderModeOptions::Final
        || settings.RenderMode == RenderSettings::RenderModeOptions::FinalAndWireframe)
        return true;
    return false;
}

void ShadowMapRenderPass::BuildRenderPackets(CommandList* commandList, ResourceTable& resources)
{
    SetRenderTargets(commandList, resources);

    for (auto light : m_drawData->Lights)
    {
        Matrix4 depthVP = BuildDepthVPMatrix(light);
        for (auto ro : m_drawData->RenderObjects)
        {
            if (!ro->GetIsVisible() || !ro->GetCastShadow())
                continue;

            Renderer::SInp::ShadowMap_sinp::CbDepthVP depthCB;
            depthCB.DepthVP = depthVP;
            depthCB.DepthVP = Renderer::GetMainCamera()->GetVP();

            ro->SetBuffer(Renderer::SInp::ShadowMap_sinp::cbDepthVPName, depthCB, m_passName);
            //ro->SetExternalCB(m_passName, Renderer::SInp::ShadowMap_sinp::cbCameraName, Renderer::GetMainCamera()->GetConstantBuffer().GetHandle());
            //ro->SetExternalCB(m_passName, Renderer::SInp::ShadowMap_sinp::cbEngineName, Renderer::EngineBuffers::GetTimeBuffer().GetHandle());

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
    }
    Texture* shadowMap = resources.GetResource("ShadowMap");
    commandList->PushCommand(RenderCommandHelpers::CreateResourceTransitonCommand(shadowMap->GetHandle(), eResourceState::PixelShaderResource, this)); // [a_vorontcov] Placeholder to prevent transition from rt to rt.
    commandList->PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
}

void ShadowMapRenderPass::Cleanup()
{

}

void ShadowMapRenderPass::SetRenderTargets(CommandList* commandList, ResourceTable& resources)
{
    SetRenderTargetsCommand cmd;
    Texture* shadowMap = resources.GetResource("ShadowMap");
    cmd.SetRenderTargets();
    cmd.RenderTargetCount = 0;

    cmd.SetDepthStencil(shadowMap->GetHandle());

    cmd.Viewport = { 0, 0, m_shadowmapSize, m_shadowmapSize };
    cmd.Scissor = { 0, 0, m_shadowmapSize, m_shadowmapSize };
    cmd.ClearDepth = true;
    cmd.ClearDepthValue = 0.f;
    cmd.ClearColor = true;
    cmd.ClearColorValue = Color::Black;
    cmd.ClearStencil = true;
    cmd.ClearStencilValue = 0;

    commandList->PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
}

Kioto::Matrix4 ShadowMapRenderPass::BuildDepthVPMatrix(Light* light)
{
    Camera m_shadowCamera{};
    Matrix4 toCamera = Matrix4::BuildLookAt(light->Position, light->Position + light->Direction, Vector3::Up);
    toCamera.SetTranslation(light->Position);
    m_shadowCamera.SetView(toCamera);
    m_shadowCamera.BuildOrtho(-30.0f, 30.0f, -30.0f, 30.0f, 0.01f, 30.0f);
    m_shadowCamera.UpdateViewProjectionMatrix();
    
    return m_shadowCamera.GetVP();
}

}