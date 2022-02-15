#include "Render/RenderPass/EditorGizmosPass.h"

#include "Core/KiotoEngine.h"
#include "Render/Camera.h"
#include "Render/Material.h"
#include "Render/Lighting/Light.h"
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

#include "Render/Shaders/autogen/sInp/GizmosImpostor.h"

namespace Kioto::Renderer
{
EditorGizmosPass::EditorGizmosPass()
    : RenderPass("EditorGizmos")
{
    Renderer::RegisterRenderPass(this);
    SetRenderTargetCount(1);

    mRenderObjects.reserve(256);

    CreateQuadMesh();
    CreateMaterial();
}

void EditorGizmosPass::SetRenderTargets(CommandList* commandList, ResourceTable& resources)
{
    bool isWireframe = KiotoCore::GetRenderSettings().RenderMode == RenderSettings::RenderModeOptions::Wireframe;
    SetRenderTargetsCommand cmd;
    TextureHandle rtHandle = mThisFrameFwdPassEnabled ? resources.GetResource("FwdTargetTexture")->GetHandle() : Renderer::DefaultBackBufferHandle;
    cmd.SetRenderTargets(rtHandle);
    cmd.RenderTargetCount = GetRenderTargetCount();
    cmd.DepthStencil = Renderer::DefaultDepthStencilHandle;

    cmd.Viewport = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
    cmd.Scissor = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
    cmd.ClearDepth = isWireframe;
    cmd.ClearDepthValue = 1.0f;
    cmd.ClearColor = isWireframe;
    if (isWireframe)
        cmd.ClearColorValue = Color::DefaultBackgroundColor;
    cmd.ClearStencil = isWireframe;
    cmd.ClearStencilValue = 0;

    commandList->PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
}

bool EditorGizmosPass::ConfigureInputsAndOutputs(ResourcesBlackboard& resources)
{
    const RenderSettings& settings = KiotoCore::GetRenderSettings();
    mThisFrameFwdPassEnabled = (settings.RenderMode == RenderSettings::RenderModeOptions::Final || settings.RenderMode == RenderSettings::RenderModeOptions::FinalAndWireframe);
    if (mThisFrameFwdPassEnabled)
        resources.ScheduleWrite("FwdTargetTexture");
    return true;
}

void EditorGizmosPass::CreateMaterial()
{
    std::string matPath = AssetsSystem::GetAssetFullPath("Materials\\GizmosImpostor.mt");
    mMaterial = new Material(matPath);
    Renderer::RegisterRenderAsset(mMaterial);
}

void EditorGizmosPass::CreateQuadMesh()
{
    mQuad = GeometryGenerator::GetFullscreenQuad();
}

EditorGizmosPass::~EditorGizmosPass()
{
    SafeDelete(mMaterial);
    for (auto ro : mRenderObjects)
        SafeDelete(ro);
    mRenderObjects.clear();
}

void EditorGizmosPass::BuildRenderPackets(CommandList* commandList, ResourceTable& resources)
{
    SetRenderTargets(commandList, resources);
    CreateNecessaryRenderObjects(mDrawData->Lights);
    for (size_t i = 0; i < mDrawData->Lights.size(); ++i)
    {
        RenderObject* ro = mRenderObjects[i];
        SInp::GizmosImpostor_sinp::ImpostorData data;
        data.position = mDrawData->Lights[i]->Position;
        data.scale = 0.5f;
        data.cutoff = 0.8f;
        data.color = Vector3(mDrawData->Lights[i]->Color.r, mDrawData->Lights[i]->Color.g, mDrawData->Lights[i]->Color.b);

        ro->SetBuffer(SInp::GizmosImpostor_sinp::impostorDataName, data, mPassName);
        ro->SetExternalCB(mPassName, Renderer::SInp::GizmosImpostor_sinp::cbCameraName, Renderer::GetMainCamera()->GetConstantBuffer().GetHandle());
        ro->SetExternalCB(mPassName, Renderer::SInp::GizmosImpostor_sinp::cbEngineName, Renderer::EngineBuffers::GetTimeBuffer().GetHandle());

        mMaterial->BuildMaterialForPass(this);

        RenderPacket currPacket = {};
        currPacket.Material = mMaterial->GetHandle();
        currPacket.Shader = mMaterial->GetPipelineState(mPassName).Shader->GetHandle();
        currPacket.TextureSet = ro->GetTextureSet(mPassName).GetHandle();
        currPacket.Mesh = mQuad->GetHandle();
        currPacket.ConstantBufferHandles = std::move(ro->GetCBHandles(mPassName));
        currPacket.Pass = GetHandle();

        commandList->PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));
    }
    commandList->PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
}

void EditorGizmosPass::CreateNecessaryRenderObjects(const std::vector<Light*>& lights)
{
    int32 diff = int32(lights.size()) - int32(mRenderObjects.size());
    if (diff <= 0)
        return;
    for (int32 i = 0; i < diff; ++i)
    {
        RenderObject* ro = new RenderObject();
        ro->SetMaterial(mMaterial);
        ro->SetMesh(mQuad);
        Renderer::RegisterRenderObject(*ro);
        mRenderObjects.push_back(ro);
    }
}

void EditorGizmosPass::Cleanup()
{
}

}