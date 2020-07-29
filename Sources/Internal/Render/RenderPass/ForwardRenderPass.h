#pragma once

#include "AssetsSystem/AssetsSystem.h"
#include "Render/RenderPass/RenderPass.h"
#include "Render/Lighting/Light.h"
#include "Render/ConstantBuffer.h"

namespace Kioto::Renderer
{
class ForwardRenderPass : public RenderPass
{
public:
    ForwardRenderPass();

    bool ConfigureInputsAndOutputs(ResourcesBlackboard& resources) override;
    void BuildRenderPackets(CommandList* commandList, ResourceTable& resources) override;
    void Cleanup() override;

private:
    void SetRenderTargets(CommandList* commandList, ResourceTable& resources) override;
    void SetPassConstantBuffers(CommandList* commandList) override;
    void SetCameraConstantBuffers(CommandList* commandList) override;

    ConstantBuffer m_lightsBuffer{ "lights", 2, 1, sizeof(Light), 256, true };
};
}