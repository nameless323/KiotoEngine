#pragma once

#include <array>
#include <variant>

#include "Core/CoreTypes.h"
#include "Math/Rect.h"
#include "Render/RendererPublic.h"
#include "Render/RenderPacket.h"
#include "Render/ScopedGpuProfiler.h"

namespace Kioto::Renderer
{
enum class eRenderCommandType
{
    eInvalidCommand,
    eSetRenderTargets,
    eSubmitConstantBuffer,
    eSubmitRenderPacket,
    eBeginGpuEvent,
    eEndGpuEvent,
    eSetGpuMarker,
    eEndRenderPass
};

struct SetRenderTargetsCommand final
{
public:
    void SetRenderTargets(
        TextureHandle rt0 = InvalidHandle,
        TextureHandle rt1 = InvalidHandle,
        TextureHandle rt2 = InvalidHandle,
        TextureHandle rt3 = InvalidHandle,
        TextureHandle rt4 = InvalidHandle,
        TextureHandle rt5 = InvalidHandle,
        TextureHandle rt6 = InvalidHandle,
        TextureHandle rt7 = InvalidHandle
    )
    {
        RenderTargetCount = 0;
        if (rt0 == InvalidHandle)
            return;

            RenderTargetCount++;
            RenderTargets[0] = rt0;

        if (rt1 == InvalidHandle)
            return;

        RenderTargetCount++;
        RenderTargets[1] = rt1;

        if (rt2 == InvalidHandle)
            return;

        RenderTargetCount++;
        RenderTargets[2] = rt2;

        if (rt3 == InvalidHandle)
            return;

        RenderTargetCount++;
        RenderTargets[3] = rt3;

        if (rt4 == InvalidHandle)
            return;

        RenderTargetCount++;
        RenderTargets[4] = rt4;

        if (rt5 == InvalidHandle)
            return;

        RenderTargetCount++;
        RenderTargets[5] = rt5;

        if (rt6 == InvalidHandle)
            return;

        RenderTargetCount++;
        RenderTargets[6] = rt6;

        if (rt7 == InvalidHandle)
            return;

        RenderTargetCount++;
        RenderTargets[7] = rt7;
    }

    TextureHandle GetRenderTarget(uint8 index) const
    {
        assert(index <= RenderTargetCount);
        return RenderTargets[index];
    }

    TextureHandle GetDepthStencil() const
    {
        return DepthStencil;
    }

    TextureHandle DepthStencil;

    RectI Scissor;
    RectI Viewport;
    bool ClearColor = true;
    bool ClearDepth = true;
    float32 ClearDepthValue = 0.0f;
    bool ClearStencil = true;
    int32 ClearStencilValue = 1;
    uint8 RenderTargetCount = -1; // 8 overloads of SetRenderTargets

private:
    std::array<TextureHandle, MaxRenderTargetsCount> RenderTargets;
};

struct SubmitConstantBufferCommand final
{
    ConstantBufferHandle BufferHandle;
    uint16 Space = 0;
    uint16 Index = 0;
};

struct SubmitRenderPacketCommand final
{
    RenderPacket Packet;
};

struct BeginGpuEventCommand
{
    std::string Name;
};

struct EndGpuEventCommand
{
};

struct SetGpuMarkerCommand
{
    std::string Name;
};

struct RenderCommand
{
    eRenderCommandType CommandType = eRenderCommandType::eInvalidCommand;
    std::variant<SetRenderTargetsCommand, SubmitConstantBufferCommand, SubmitRenderPacketCommand, BeginGpuEventCommand, EndGpuEventCommand, SetGpuMarkerCommand> Command;

    std::string PassName; // [a_vorontcov] For debugging.
};

struct PassEndsCommand
{};

namespace RenderCommandHelpers
{
RenderCommand CreateConstantBufferCommand(const ConstantBuffer& buffer, RenderPass* pass);
RenderCommand CreateRenderPacketCommand(RenderPacket packet, RenderPass* pass);
RenderCommand CreateSetRenderTargetCommand(SetRenderTargetsCommand setRTCmd, RenderPass* pass);
RenderCommand CreateSetRenderTargetCommand(SetRenderTargetsCommand setRTCmd, RenderPass* pass);
RenderCommand CreatePassEndsCommand(RenderPass* pass);
RenderCommand CreateBeginGpuEventCommand(std::string name);
RenderCommand CreateEndGpuEventCommand();
RenderCommand CreateGpuMarkerCommand(std::string name);

#define SCOPED_GPU_EVENT(name) ScopedGpuProfiler ____scopedProfiler___ ## __LINE__(this, name);
}
}