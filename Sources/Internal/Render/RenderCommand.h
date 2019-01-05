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
};

namespace RenderCommandHelpers
{
RenderCommand CreateConstantBufferCommand(ConstantBufferHandle handle)
{
    SubmitConstantBufferCommand cbCmd;
    cbCmd.BufferHandle = handle;

    RenderCommand command;
    command.CommandType = eRenderCommandType::eSubmitConstantBuffer;
    command.Command = cbCmd;

    return command;
}

RenderCommand CreateRenderPacketCommand(RenderPacket packet)
{
    SubmitRenderPacketCommand rpCommand;
    rpCommand.Packet = packet;

    RenderCommand command;
    command.CommandType = eRenderCommandType::eSubmitRenderPacket;
    command.Command = rpCommand;

    return command;
}

RenderCommand CreateSetRenderTargetCommand(SetRenderTargetsCommand setRTCmd)
{
    RenderCommand command;
    command.CommandType = eRenderCommandType::eSetRenderTargets;
    command.Command = setRTCmd;

    return command;
}

}
}