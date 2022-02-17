#pragma once

#include "AssetsSystem/AssetsSystem.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{
class EditorGizmosPass : public RenderPass
{
public:
    EditorGizmosPass();
    ~EditorGizmosPass();

    bool ConfigureInputsAndOutputs(ResourcesBlackboard& resources) override;
    void BuildRenderPackets(CommandList* commandList, ResourceTable& resources) override;
    void Cleanup() override;

private:
    void SetRenderTargets(CommandList* commandList, ResourceTable& resources) override;

    void CreateNecessaryRenderObjects(const std::vector<Light*>& lights);

    void CreateMaterial();
    void CreateQuadMesh();

    Material* mMaterial = nullptr;
    Mesh* mQuad = nullptr;
    std::vector<RenderObject*> mRenderObjects;

    bool mThisFrameFwdPassEnabled = true;
};
}