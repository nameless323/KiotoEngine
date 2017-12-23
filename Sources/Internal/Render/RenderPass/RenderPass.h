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

enum PassPriority
{
    MainPass = 100
};

class RenderPass
{
public:
    RenderPass() = default;
    RenderPass(const RenderPass& other);
    RenderPass& operator= (const RenderPass& other);

    void SetScissor(const RectI scissor);
    void SetViewport(const RectI viewport);
    void SetClearColor(bool clearColor);
    void SetClearDepth(bool clearDepth);
    void SetClearDepthValue(float32 clearDepthValue);
    void SetClearStencil(bool clearStencil);
    void SetClearStencilValue(int32 clearStencilValue);
    void SetRenderTarget(uint32 index, TextureHandle tex);
    void SetDepthStencil(TextureHandle depthStencil);
    void SetPriority(uint32 priority);

    RectI GetScissor() const;
    RectI GetViewport() const;
    bool GetClearColor() const;
    bool GetClearDepth() const;
    float32 GetClearDepthValue() const;
    bool GetClearStencil() const;
    int32 GetClearStencilValue() const;
    TextureHandle GetRenderTarget(int32 index) const;
    TextureHandle GetDepthStencil() const;
    uint32 GetPriority() const;

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
    uint32 m_priority = PassPriority::MainPass;
};

inline void RenderPass::SetScissor(const RectI scissor)
{
    m_scissor = scissor;
}

inline void RenderPass::SetViewport(const RectI viewport)
{
    m_viewport = viewport;
}

inline void RenderPass::SetClearColor(bool clearColor)
{
    m_clearColor = clearColor;
}

inline void RenderPass::SetClearDepth(bool clearDepth)
{
    m_clearDepth = clearDepth;
}

inline void RenderPass::SetClearDepthValue(float32 clearDepthValue)
{
    m_clearDepthValue = clearDepthValue;
}

inline void RenderPass::SetClearStencil(bool clearStencil)
{
    m_clearStencilValue = clearStencil;
}

inline void RenderPass::SetClearStencilValue(int32 clearStencilValue)
{
    m_clearStencilValue = clearStencilValue;
}

inline void RenderPass::SetRenderTarget(uint32 index, TextureHandle tex)
{
    m_renderTargets[index] = tex;
}

inline void RenderPass::SetDepthStencil(TextureHandle depthStencil)
{
    m_depthStencil = depthStencil;
}

inline void RenderPass::SetPriority(uint32 priority)
{
    m_priority = priority;
}

inline RectI RenderPass::GetScissor() const
{
    return m_scissor;
}

inline RectI RenderPass::GetViewport() const
{
    return m_viewport;
}

inline bool RenderPass::GetClearColor() const
{
    return m_clearColor;
}

inline bool RenderPass::GetClearDepth() const
{
    return m_clearDepth;
}

inline float32 RenderPass::GetClearDepthValue() const
{
    return m_clearDepthValue;
}

inline bool RenderPass::GetClearStencil() const
{
    return m_clearStencil;
}

inline int32 RenderPass::GetClearStencilValue() const
{
    return m_clearStencilValue;
}

inline TextureHandle RenderPass::GetRenderTarget(int32 index) const
{
    return m_renderTargets[index];
}

inline TextureHandle RenderPass::GetDepthStencil() const
{
    return m_depthStencil;
}
}