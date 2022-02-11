#include "Render/RenderPass/RenderPass.h"

#include "Render/Renderer.h"

namespace Kioto::Renderer
{
RenderPass::RenderPass(const RenderPass& other)
    : m_scissor(other.m_scissor)
    , m_viewport(other.m_viewport)
    , m_clearColor(other.m_clearColor)
    , m_clearDepth(other.m_clearDepth)
    , m_clearDepthValue(other.m_clearDepthValue)
    , m_clearStencil(other.m_clearStencil)
    , m_clearStencilValue(other.m_clearStencilValue)
    , m_renderTargets(other.m_renderTargets)
    , m_depthStencil(other.m_depthStencil)
    , m_priority(other.m_priority)
    , m_handle(other.m_handle)
    , m_renderTargetCount(other.m_renderTargetCount)
    , m_passName(other.m_passName)
{
}
}