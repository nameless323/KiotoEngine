#pragma once

#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{
    class WireframeRenderPass : public RenderPass
    {
    public:
        WireframeRenderPass();

        bool ConfigureInputsAndOutputs(ResourcesBlackboard& resources) override;
        void BuildRenderPackets() override;
        void Cleanup() override;

    private:
        void SetRenderTargets() override;
        void SetPassConstantBuffers() override;
        void SetCameraConstantBuffers() override;
    };
}
