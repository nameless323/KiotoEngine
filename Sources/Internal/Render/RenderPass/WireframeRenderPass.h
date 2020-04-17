#pragma once

#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{
    class WireframeRenderPass : public RenderPass
    {
    public:
        WireframeRenderPass();

        void CollectRenderData() override;
        void Cleanup() override;

    private:
        void SetRenderTargets() override;
        void SetPassConstantBuffers() override;
        void SetCameraConstantBuffers() override;
    };
}
