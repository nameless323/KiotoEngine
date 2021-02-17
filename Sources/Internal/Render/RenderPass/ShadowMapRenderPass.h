#pragma once

#include "Render/RenderPass/RenderPass.h"
#include "Render/Camera.h"

namespace Kioto::Renderer
{
struct Light;

class ShadowMapRenderPass : public RenderPass
{
public:
    ShadowMapRenderPass();

    bool ConfigureInputsAndOutputs(ResourcesBlackboard& resources) override;
    void BuildRenderPackets(CommandList* commandList, ResourceTable& resources) override;
    void Cleanup() override;

private:
    void SetRenderTargets(CommandList* commandList, ResourceTable& resources) override;
    Matrix4 BuildDepthVPMatrix(Light* light);

    Mesh* m_debugQuad = nullptr;
    int32 m_shadowmapSize = 1024;
};
}