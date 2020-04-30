#pragma once

#include "AssetsSystem/AssetsSystem.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{
class ForwardRenderPass : public RenderPass
{
public:
    ForwardRenderPass();

    bool ConfigureInputsAndOutputs(ResourcesBlackboard& resources) override;
    void BuildRenderPackets(CommandList* commandList, ResourcesBlackboard& resources) override;
    void Cleanup() override;

private:
    void SetRenderTargets(CommandList* commandList) override;
    void SetPassConstantBuffers(CommandList* commandList) override;
    void SetCameraConstantBuffers(CommandList* commandList) override;
};
}