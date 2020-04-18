#pragma once

#include <array>
#include <vector>

#include "Core/CoreTypes.h"
#include "Math/Rect.h"
#include "Render/RenderCommand.h"
#include "Render/RendererPublic.h"

namespace Kioto::Renderer
{
enum PassPriority
{
    MainPass = 100
};

class RenderObject;

class RenderPass // [a_vorontcov] Add render target and depth stencil resource state before and after.
{
public:
    RenderPass(std::string name)
        : m_passName(name)
    {
        m_renderObjects.resize(2048);
    }

    RenderPass(const RenderPass& other);

    virtual ~RenderPass() = default;

    // Describe what resources pass get for input and what resources it writes.
    // Return true - pass will be executed in the current frame. false - otherwise.
    virtual bool ConfigureInputsAndOutputs() abstract;

    virtual void Setup() // set all pass buffers
    {
        PushCommand(RenderCommandHelpers::CreateBeginGpuEventCommand(m_passName));
        SetPassConstantBuffers();
        SetCameraConstantBuffers();
    }

    virtual void CollectRenderData() abstract;
    virtual void SubmitRenderData();

    virtual void Cleanup() abstract; // cleanup all pass setups

    void PushCommand(RenderCommand command)
    {
        m_commands.push_back(command);
    }

    const std::vector<RenderCommand>& GetRenderCommands() const
    {
        return m_commands;
    }

    void ClearCommands()
    {
        m_commands.clear();
    }

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
    void SetRenderObjects(std::vector<RenderObject*> renderObjects);

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
    virtual void SetRenderTargets() abstract; // Set scissor, render targets, viewports
    virtual void SetPassConstantBuffers() abstract;
    virtual void SetCameraConstantBuffers() abstract;

    RectI m_scissor;
    RectI m_viewport;
    bool m_clearColor = true;
    bool m_clearDepth = true;
    float32 m_clearDepthValue = 0.0f;
    bool m_clearStencil = true;
    int32 m_clearStencilValue = 1;
    uint8 m_renderTargetCount = -1;
    RenderPassHandle m_handle = InvalidHandle;
    std::array<TextureHandle, MaxRenderTargetsCount> m_renderTargets;
    TextureHandle m_depthStencil;
    uint32 m_priority = PassPriority::MainPass;

    std::string m_passName;
    std::vector<RenderCommand> m_commands;
    std::vector<RenderObject*> m_renderObjects; // [a_vorontcov] TODO: think maybe just a pointer will be fine.
};

inline void RenderPass::SetScissor(const RectI& scissor)
{
    m_scissor = scissor;
}

inline void RenderPass::SetViewport(const RectI& viewport)
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

inline void RenderPass::SetRenderTargetCount(uint8 count)
{
    m_renderTargetCount = count;
}

inline void RenderPass::SetHandle(RenderPassHandle handle)
{
    m_handle = handle;
}

inline void RenderPass::SetRenderObjects(std::vector<RenderObject*> renderObjects)
{
    m_renderObjects = std::move(renderObjects);
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

inline uint32 RenderPass::GetPriority() const
{
    return m_priority;
}

inline uint8 RenderPass::GetRenderTargetCount() const
{
    return m_renderTargetCount;
}

inline RenderPassHandle RenderPass::GetHandle() const
{
    return m_handle;
}

inline const std::string& RenderPass::GetName() const
{
    return m_passName;
}
}