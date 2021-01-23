#pragma once

#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{
class ShadowMapRenderPass : public RenderPass
{
public:
    ShadowMapRenderPass();

    bool ConfigureInputsAndOutputs(ResourcesBlackboard& resources) override;
    void BuildRenderPackets(CommandList* commandList, ResourceTable& resources) override;
    void Cleanup() override;

private:
    void SetRenderTargets(CommandList* commandList, ResourceTable& resources) override;

    Mesh* m_debugQuad = nullptr;
    int32 m_shadowmapSize = 1024;
};
}