#include "stdafx.h"

#include "Render/RenderPass/EditorGizmosPass.h"

#include "Core/KiotoEngine.h"
#include "Render/Camera.h"
#include "Render/Material.h"
#include "Render/Lighting/Light.h"
#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderObject.h"
#include "Render/RenderPacket.h"
#include "Render/RenderOptions.h"
#include "Render/Shader.h"
#include "Render/RenderGraph/ResourcesBlackboard.h"
#include "Render/RenderGraph/ResourceTable.h"
#include "Render/Geometry/GeometryGenerator.h"
#include "Render/Renderer.h"

#include "Render/Shaders/autogen/sInp/GizmosImpostor.h"

namespace Kioto::Renderer
{
EditorGizmosPass::EditorGizmosPass()
    : RenderPass("EditorGizmos")
{
    Renderer::RegisterRenderPass(this);
    SetRenderTargetCount(1);

    m_renderObjects.reserve(256);

    CreateQuadMesh();
    CreateMaterial();
}

void EditorGizmosPass::SetRenderTargets(CommandList* commandList, ResourceTable& resources)
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

bool EditorGizmosPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    return true;
}

void EditorGizmosPass::CreateMaterial()
{
    std::string matPath = AssetsSystem::GetAssetFullPath("Materials\\GizmosImpostor.mt");
    m_material = new Material(matPath);
    Renderer::RegisterRenderAsset(m_material);
}

void EditorGizmosPass::CreateQuadMesh()
{
    m_quad = GeometryGenerator::GetFullscreenQuad();
}

EditorGizmosPass::~EditorGizmosPass()
{
    SafeDelete(m_material);
    for (auto ro : m_renderObjects)
        SafeDelete(ro);
    m_renderObjects.clear();
}

void EditorGizmosPass::BuildRenderPackets(CommandList* commandList, ResourceTable& resources)
{
    CreateNecessaryRenderObjects(m_drawData->Lights);
    for (size_t i = 0; i < m_drawData->Lights.size(); ++i)
    {
        RenderObject* ro = m_renderObjects[i];
        SInp::GizmosImpostor_sinp::ImpostorData data;
        data.position = m_drawData->Lights[i]->Position;
        data.scale = 1.0f;

        ro->SetBuffer(SInp::GizmosImpostor_sinp::impostorDataName, data, m_passName);
        ro->SetExternalCB(m_passName, Renderer::SInp::GizmosImpostor_sinp::cbCameraName, Renderer::GetMainCamera()->GetConstantBuffer().GetHandle());
        ro->SetExternalCB(m_passName, Renderer::SInp::GizmosImpostor_sinp::cbEngineName, Renderer::EngineBuffers::GetTimeBuffer().GetHandle());

        m_material->BuildMaterialForPass(this);

        RenderPacket currPacket = {};
        currPacket.Material = m_material->GetHandle();
        currPacket.Shader = m_material->GetPipelineState(m_passName).Shader->GetHandle();
        currPacket.TextureSet = ro->GetTextureSet(m_passName).GetHandle();
        currPacket.Mesh = m_quad->GetHandle();
        currPacket.ConstantBufferHandles = std::move(ro->GetCBHandles(m_passName));
        currPacket.Pass = GetHandle();

        commandList->PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));
    }
    commandList->PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
}

void EditorGizmosPass::CreateNecessaryRenderObjects(const std::vector<Light*>& lights)
{
    int32 diff = int32(lights.size()) - int32(m_renderObjects.size());
    if (diff <= 0)
        return;
    for (int32 i = 0; i < diff; ++i)
    {
        RenderObject* ro = new RenderObject();
        ro->SetMaterial(m_material);
        ro->SetMesh(m_quad);
        Renderer::RegisterRenderObject(*ro);
        m_renderObjects.push_back(ro);
    }
}

void EditorGizmosPass::Cleanup()
{
}

}