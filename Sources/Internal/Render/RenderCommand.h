//
// Copyright (C) Aleksandr Vorontcov. 2019
// Distributed under the MIT License (license terms are at http://opensource.org/licenses/MIT).
//

#pragma once

#include <array>
#include <variant>

#include "Core/CoreTypes.h"
#include "Math/Rect.h"
#include "Render/RendererPublic.h"
#include "Render/DX12/RenderPacket.h" // ?????

namespace Kioto::Renderer
{
enum class eRenderCommandType
{
    eInvalidCommand,
    eSetRenderTargets,
    eSubmitConstantBuffer,
    eSubmitRenderPacket
};

struct SetRenderTargetsCommand final
{
    std::array<TextureHandle, MaxRenderTargetsCount> RenderTargets;
    TextureHandle DepthStencil;

    RectI Scissor;
    RectI Viewport;
    bool ClearColor = true;
    bool ClearDepth = true;
    float32 ClearDepthValue = 0.0f;
    bool ClearStencil = true;
    int32 ClearStencilValue = 1;
    uint8 RenderTargetCount = -1;
};

struct SubmitConstantBufferCommand final
{
    ConstantBufferHandle BufferHandle;
};

struct SubmitRenderPacketCommand final
{
    RenderPacket Packet;
};

struct RenderCommand
{
    eRenderCommandType CommandType = eRenderCommandType::eInvalidCommand;
    std::variant<SetRenderTargetsCommand, SubmitConstantBufferCommand, SubmitRenderPacketCommand> Command;

    std::string PassName; // [a_vorontcov] For debugging.
};

namespace RenderCommandHelpers
{
RenderCommand CreateConstantBufferCommand(ConstantBufferHandle handle, RenderPass* pass);
RenderCommand CreateRenderPacketCommand(RenderPacket packet, RenderPass* pass);
RenderCommand CreateSetRenderTargetCommand(SetRenderTargetsCommand setRTCmd, RenderPass* pass);

}
}