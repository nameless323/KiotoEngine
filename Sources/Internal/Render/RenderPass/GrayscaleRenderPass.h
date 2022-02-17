#pragma once

#include "AssetsSystem/AssetsSystem.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{
class GrayscaleRenderPass : public RenderPass
{
public:
    GrayscaleRenderPass();
    ~GrayscaleRenderPass();

    bool ConfigureInputsAndOutputs(ResourcesBlackboard& resources) override;
    void BuildRenderPackets(CommandList* commandList, ResourceTable& resources) override;
    void Cleanup() override;

private:
    void SetRenderTargets(CommandList* commandList, ResourceTable& resources) override;

    void CreateMaterial();
    void CreateQuadMesh();

    Material* mMaterial = nullptr;
    Mesh* mQuad = nullptr;
    RenderObject* mRenderObject = nullptr;
};
}