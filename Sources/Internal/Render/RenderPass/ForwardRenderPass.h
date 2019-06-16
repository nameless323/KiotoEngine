//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "AssetsSystem/AssetsSystem.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{
class ForwardRenderPass : public RenderPass
{
public:
    ForwardRenderPass();

    void CollectRenderData() override; 
    void Cleanup() override;

private:
    void SetRenderTargets() override;
    void SetPassConstantBuffers() override;
    void SetCameraConstantBuffers() override;
 
    void UpdateBuffer();

    std::string m_matPath = AssetsSystem::GetAssetFullPath("Materials\\Test.mt");
    Material* m_material = nullptr;
    Material* m_materialCopy = nullptr;
    std::string m_meshPath = AssetsSystem::GetAssetFullPath(R"(Models\Teapot.fbx)");
    Mesh* m_mesh = nullptr;
    Mesh* m_meshCopy = nullptr;

    /*std::string m_spherePath = AssetsSystem::GetAssetFullPath(R"(Models\Sphere.fbx)");
    std::string m_matPath = AssetsSystem::GetAssetFullPath("Materials\\TestRed.mt");
    Renderer::Material* m_materialRed = nullptr;
    Renderer::Mesh* m_meshSphere = nullptr;*/
};
}