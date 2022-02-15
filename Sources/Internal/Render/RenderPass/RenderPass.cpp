#include "Render/RenderPass/RenderPass.h"

#include "Render/Renderer.h"

namespace Kioto::Renderer
{
RenderPass::RenderPass(const RenderPass& other)
    : mScissor(other.mScissor)
    , mViewport(other.mViewport)
    , mClearColor(other.mClearColor)
    , mClearDepth(other.mClearDepth)
    , mClearDepthValue(other.mClearDepthValue)
    , mClearStencil(other.mClearStencil)
    , mClearStencilValue(other.mClearStencilValue)
    , mRenderTargets(other.mRenderTargets)
    , mDepthStencil(other.mDepthStencil)
    , mPriority(other.mPriority)
    , mHandle(other.mHandle)
    , mRenderTargetCount(other.mRenderTargetCount)
    , mPassName(other.mPassName)
{
}
}