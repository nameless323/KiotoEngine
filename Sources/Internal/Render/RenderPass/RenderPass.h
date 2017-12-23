//
// Copyright (C) Alexandr Vorontsov 2017.
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <Array>

#include "Core/CoreTypes.h"
#include "Math/Rect.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
class RenderPass
{
public:
    RenderPass() = default;
    RenderPass(const RenderPass& other);
    RenderPass& operator= (const RenderPass& other);

private:
    RectI m_scissor;
    RectI m_viewport;
    bool m_clearColor = true;
    bool m_clearDepth = true;
    float32 m_clearDepthValue = 0.0f;
    bool m_clearStencil = true;
    int32 m_clearStencilValue = 1;
    std::array<TextureHandle, 4> m_renderTargets;
    TextureHandle m_depthStencil;
};
}