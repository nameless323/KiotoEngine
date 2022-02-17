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

    struct Lights
    {
        SInp::Light Light[256];
        Matrix4 ShadowTransform;
    };
    ConstantBuffer mLightsBuffer{ "lights", 2, 1, sizeof(Lights), 1, true };
    Lights mLights;
};
}