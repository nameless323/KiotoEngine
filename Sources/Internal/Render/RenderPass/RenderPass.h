#pragma once

#include <array>
#include <vector>

#include "Core/CoreTypes.h"
#include "Math/Rect.h"
#include "Render/RenderCommand.h"
#include "Render/RendererPublic.h"
#include "Render/RenderPass/DrawData.h"

namespace Kioto::Renderer
{
class ResourcesBlackboard;
class ResourceTable;

enum PassPriority
{
    MainPass = 100
};

class RenderObject;

class RenderPass // [a_vorontcov] Add render target and depth stencil resource state before and after.
{
public:
    RenderPass(std::string name)
        : mPassName(name)
    {
    }

    RenderPass(const RenderPass& other);

    virtual ~RenderPass() = default;

    // Describe what resources pass get for input and what resources it writes.
    // Return true - pass will be executed in the current frame. false - otherwise.
    virtual bool ConfigureInputsAndOutputs(ResourcesBlackboard& resources) abstract;

    virtual void Setup()
    {}

    virtual void BuildRenderPackets(CommandList* commandList, ResourceTable& resources) abstract;

    virtual void Cleanup() abstract; // cleanup all pass setups

    void SetScissor(const RectI& scissor);
    void SetViewport(const RectI& viewport);
    void SetClearColor(bool clearColor);
    void SetClearDepth(bool clearDepth);
    void SetClearDepthValue(float32 clearDepthValue);
    void SetClearStencil(bool clearStencil);
    void SetClearStencilValue(int32 clearStencilValue);
    void SetRenderTarget(uint32 index, TextureHandle tex);
    void SetDepthStencil(TextureHandle depthStencil);
    void SetPriority(uint32 priority);
    void SetRenderTargetCount(uint8 count);
    void SetHandle(RenderPassHandle handle);
    void SetDrawData(const DrawData* drawData);

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
    uint8 GetRenderTargetCount() const;
    RenderPassHandle GetHandle() const;

    const std::string& GetName() const;

protected:
    virtual void SetRenderTargets(CommandList* commandList, ResourceTable& resources) abstract; // Set scissor, render targets, viewports

    RectI mScissor;
    RectI mViewport;
    bool mClearColor = true;
    bool mClearDepth = true;
    float32 mClearDepthValue = 0.0f;
    bool mClearStencil = true;
    int32 mClearStencilValue = 1;
    uint8 mRenderTargetCount = -1;
    RenderPassHandle mHandle = InvalidHandle;
    std::array<TextureHandle, MaxRenderTargetsCount> mRenderTargets;
    TextureHandle mDepthStencil;
    uint32 mPriority = PassPriority::MainPass;

    std::string mPassName;
    const DrawData* mDrawData = nullptr;
};

inline void RenderPass::SetScissor(const RectI& scissor)
{
    mScissor = scissor;
}

inline void RenderPass::SetViewport(const RectI& viewport)
{
    mViewport = viewport;
}

inline void RenderPass::SetClearColor(bool clearColor)
{
    mClearColor = clearColor;
}

inline void RenderPass::SetClearDepth(bool clearDepth)
{
    mClearDepth = clearDepth;
}

inline void RenderPass::SetClearDepthValue(float32 clearDepthValue)
{
    mClearDepthValue = clearDepthValue;
}

inline void RenderPass::SetClearStencil(bool clearStencil)
{
    mClearStencilValue = clearStencil;
}

inline void RenderPass::SetClearStencilValue(int32 clearStencilValue)
{
    mClearStencilValue = clearStencilValue;
}

inline void RenderPass::SetRenderTarget(uint32 index, TextureHandle tex)
{
    mRenderTargets[index] = tex;
}

inline void RenderPass::SetDepthStencil(TextureHandle depthStencil)
{
    mDepthStencil = depthStencil;
}

inline void RenderPass::SetPriority(uint32 priority)
{
    mPriority = priority;
}

inline void RenderPass::SetRenderTargetCount(uint8 count)
{
    mRenderTargetCount = count;
}

inline void RenderPass::SetHandle(RenderPassHandle handle)
{
    mHandle = handle;
}

inline void RenderPass::SetDrawData(const DrawData* drawData)
{
    mDrawData = drawData;
}

inline RectI RenderPass::GetScissor() const
{
    return mScissor;
}

inline RectI RenderPass::GetViewport() const
{
    return mViewport;
}

inline bool RenderPass::GetClearColor() const
{
    return mClearColor;
}

inline bool RenderPass::GetClearDepth() const
{
    return mClearDepth;
}

inline float32 RenderPass::GetClearDepthValue() const
{
    return mClearDepthValue;
}

inline bool RenderPass::GetClearStencil() const
{
    return mClearStencil;
}

inline int32 RenderPass::GetClearStencilValue() const
{
    return mClearStencilValue;
}

inline TextureHandle RenderPass::GetRenderTarget(int32 index) const
{
    return mRenderTargets[index];
}

inline TextureHandle RenderPass::GetDepthStencil() const
{
    return mDepthStencil;
}

inline uint32 RenderPass::GetPriority() const
{
    return mPriority;
}

inline uint8 RenderPass::GetRenderTargetCount() const
{
    return mRenderTargetCount;
}

inline RenderPassHandle RenderPass::GetHandle() const
{
    return mHandle;
}

inline const std::string& RenderPass::GetName() const
{
    return mPassName;
}
}