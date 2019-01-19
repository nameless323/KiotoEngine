//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "Render/Renderer.h"
#include "Render/RenderCommand.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/Material.h"

namespace Kioto::Renderer
{
class ForwardRenderPass : public RenderPass
{
public:
    ForwardRenderPass()
        : RenderPass("ForwardPass")
    {
        m_material = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Renderer::Material>(m_matPath);
        m_mesh = AssetsSystem::GetRenderAssetsManager()->GetOrLoadAsset<Renderer::Mesh>(m_meshPath);

        Renderer::RegisterRenderPass(this);
        SetRenderTargetCount(1);

        m_material->BuildMaterialForPass(this);

    }

    virtual void CollectRenderData() override
    {
        SetRenderTargets();
        UpdateBuffer();

        Renderer::RenderPacket currPacket;
        currPacket.Material = m_material->GetHandle();
        currPacket.Shader = m_material->GetShader()->GetHandle();
        currPacket.TextureSet = m_material->GetShaderData().textureSet.GetHandle();
        currPacket.Mesh = m_mesh->GetHandle();
        currPacket.Pass = GetHandle();
        currPacket.CBSet = m_material->GetShaderData().bufferSetHandle;

        PushCommand(RenderCommandHelpers::CreateRenderPacketCommand(currPacket, this));

        PushCommand(RenderCommandHelpers::CreatePassEndsCommand(this));
    }

    virtual void SubmitRenderData() override
    {
        Renderer::SubmitRenderCommands(GetRenderCommands());
        ClearCommands();
    }
 
    virtual void Cleanup() override
    {
        //throw std::logic_error("The method or operation is not implemented.");
    }

private:
    void UpdateBuffer()
    {
        static float32 angle = 0.0f;
        //angle += GlobalTimer::GetDeltaTime();	
        angle += 0.01f;
        Matrix4 toWorld = Matrix4::BuildRotation(Vector3(1.0f, 1.0f, 0.0f).Normalize(), angle);
        //toWorld = Matrix4::Identity();	
        toWorld.SetTranslation({ 0.0f, 0.0f, 2.0f });

        Matrix4 toModel;
        toWorld.Inversed(toModel);

        m_material->SetValueToBuffer("ToModel", toModel.Tranposed());
        m_material->SetValueToBuffer("ToWorld", toWorld.Tranposed());
    }

    virtual void SetRenderTargets() override
    {
        SetRenderTargetsCommand cmd;
        cmd.SetRenderTargets(Renderer::DefaultBackBufferHandle);
        cmd.RenderTargetCount = GetRenderTargetCount();
        cmd.DepthStencil = Renderer::DefaultDepthStencilHandle;

        cmd.Viewport = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
        cmd.Scissor = { 0, 0, Renderer::GetWidth(), Renderer::GetHeight() };
        cmd.ClearDepth = true;
        cmd.ClearDepthValue = 0.0f;
        cmd.ClearColor = true;
        cmd.ClearStencil = true;
        cmd.ClearStencilValue = 0;

        PushCommand(RenderCommandHelpers::CreateSetRenderTargetCommand(cmd, this));
    }

    virtual void SetPassConstantBuffers() override
    {
        //throw std::logic_error("The method or operation is not implemented.");
    }

    virtual void SetCameraConstantBuffers() override
    {
        PushCommand(RenderCommandHelpers::CreateConstantBufferCommand(Renderer::GetMainCamera()->GetConstantBuffer(), this));
    }
 
    std::string m_matPath = AssetsSystem::GetAssetFullPath("Materials\\Test.mt");
    Renderer::Material* m_material = nullptr;
    Renderer::Material* m_materialCopy = nullptr;
    std::string m_meshPath = AssetsSystem::GetAssetFullPath(R"(Models\Teapot.fbx)");
    Renderer::Mesh* m_mesh = nullptr;
    Renderer::Mesh* m_meshCopy = nullptr;

    /*std::string m_spherePath = AssetsSystem::GetAssetFullPath(R"(Models\Sphere.fbx)");
    std::string m_matPath = AssetsSystem::GetAssetFullPath("Materials\\TestRed.mt");
    Renderer::Material* m_materialRed = nullptr;
    Renderer::Mesh* m_meshSphere = nullptr;*/
};
}