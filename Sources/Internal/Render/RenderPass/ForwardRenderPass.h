//
// Copyright (C) Aleksandr Vorontcov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include "AssetsSystem/AssetsSystem.h"
#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer
{
class ForwardRenderPass : public RenderPass
{
public:
    ForwardRenderPass();

    void CollectRenderData() override; 
    void Cleanup() override;

private:
    void SetRenderTargets() override;
    void SetPassConstantBuffers() override;
    void SetCameraConstantBuffers() override;
};
}