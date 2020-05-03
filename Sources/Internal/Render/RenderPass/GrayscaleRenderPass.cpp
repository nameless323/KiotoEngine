#include "stdafx.h"

#include "Render/RenderPass/GrayscaleRenderPass.h"

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
#include "Render/Geometry/GeometryGenerator.h"

namespace Kioto::Renderer
{
GrayscaleRenderPass::GrayscaleRenderPass()
    : RenderPass("Grayscale")
{
    Renderer::RegisterRenderPass(this);
    SetRenderTargetCount(1);

    m_renderObject = new RenderObject();
    m_renderObject->SetMaterial(m_material);
    m_renderObject->SetMesh(m_quad);
    Renderer::RegisterRenderObject(*m_renderObject);
}

void GrayscaleRenderPass::BuildRenderPackets(CommandList* commandList, ResourceTable& resources)
{
    SetRenderTargets(commandList, resources);
    Texture* input = resources.GetResource("FwdTargetTexture");

    Material* mat = m_renderObject->GetMaterial();
    Mesh* mesh = m_renderObject->GetMesh();
    mat->BuildMaterialForPass(this);

    RenderPacket currPacket = {};
    currPacket.Material = mat->GetHandle();
    currPacket.Shader = mat->GetPipelineState(m_passName).Shader->GetHandle();
    currPacket.TextureSet = m_renderObject->GetTextureSet(m_passName).GetHandle();
    currPacket.Mesh = mesh->GetHandle();
    currPacket.Pass = GetHandle();
    currPacket.CBSet = m_renderObject->GetBufferLayout(m_passName).bufferSetHandle;

    commandList->PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));

    commandList->PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
}

void GrayscaleRenderPass::Cleanup()
{
}

void GrayscaleRenderPass::SetRenderTargets(CommandList* commandList, ResourceTable& resources)
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
    cmd.ClearStencil = false;
    cmd.ClearStencilValue = 0;

    commandList->PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
}

void GrayscaleRenderPass::SetPassConstantBuffers(CommandList* commandList)
{
}

void GrayscaleRenderPass::SetCameraConstantBuffers(CommandList* commandList)
{
}

bool GrayscaleRenderPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    const RenderOptions& settings = KiotoCore::GetRenderSettings();

    resources.ScheduleRead("FwdTargetTexture");

    if (settings.RenderMode == RenderOptions::RenderModeOptions::Final
        || settings.RenderMode == RenderOptions::RenderModeOptions::FinalAndWireframe)
        return true;
    return false;
}

void GrayscaleRenderPass::CreateMaterial()
{

}

void GrayscaleRenderPass::CreateQuadMesh()
{
    m_quad = GeometryGenerator::GetFullscreenQuad();
}

GrayscaleRenderPass::~GrayscaleRenderPass()
{
    SafeDelete(m_material);
    SafeDelete(m_renderObject);
}

}