#include "Render/RenderPass/GrayscaleRenderPass.h"

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
#include "Render/Geometry/GeometryGenerator.h"
#include "Render/Geometry/Mesh.h"
#include "Render/Renderer.h"

#include "Render/Shaders/autogen/sInp/Grayscale.h"

namespace Kioto::Renderer
{
GrayscaleRenderPass::GrayscaleRenderPass()
    : RenderPass("Grayscale")
{
    Renderer::RegisterRenderPass(this);
    SetRenderTargetCount(1);

    CreateQuadMesh();
    CreateMaterial();

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
    m_renderObject->SetExternalCB(m_passName, Renderer::SInp::Grayscale_sinp::cbCameraName, Renderer::GetMainCamera()->GetConstantBuffer().GetHandle());
    m_renderObject->SetExternalCB(m_passName, Renderer::SInp::Grayscale_sinp::cbEngineName, Renderer::EngineBuffers::GetTimeBuffer().GetHandle());

    mat->BuildMaterialForPass(this);

    m_renderObject->SetTexture("InputColor", input, m_passName);

    RenderPacket currPacket = {};
    currPacket.Material = mat->GetHandle();
    currPacket.Shader = mat->GetPipelineState(m_passName).Shader->GetHandle();
    currPacket.TextureSet = m_renderObject->GetTextureSet(m_passName).GetHandle();
    currPacket.Mesh = mesh->GetHandle();
    currPacket.Pass = GetHandle();
    currPacket.ConstantBufferHandles = std::move(m_renderObject->GetCBHandles(m_passName));

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

bool GrayscaleRenderPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    const RenderSettings& settings = KiotoCore::GetRenderSettings();

    resources.ScheduleRead("FwdTargetTexture");

    if (settings.RenderMode == RenderSettings::RenderModeOptions::Final
        || settings.RenderMode == RenderSettings::RenderModeOptions::FinalAndWireframe)
        return true;
    return false;
}

void GrayscaleRenderPass::CreateMaterial()
{
    std::string matPath = AssetsSystem::GetAssetFullPath("Materials\\Grayscale.mt");
    m_material = new Material(matPath);
    Renderer::RegisterRenderAsset(m_material);
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