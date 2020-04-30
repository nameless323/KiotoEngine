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
    void BuildRenderPackets() override;
    void Cleanup() override;

private:
    void SetRenderTargets() override;
    void SetPassConstantBuffers() override;
    void SetCameraConstantBuffers() override;
};
}