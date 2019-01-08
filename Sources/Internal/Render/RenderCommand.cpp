#include "stdafx.h"

#include "Render/RenderCommand.h"

#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer::RenderCommandHelpers
{
RenderCommand CreateConstantBufferCommand(ConstantBufferHandle handle, RenderPass* pass)
{
    SubmitConstantBufferCommand cbCmd;
    cbCmd.BufferHandle = handle;

    RenderCommand command;
    command.CommandType = eRenderCommandType::eSubmitConstantBuffer;
    command.Command = cbCmd;

    command.PassName = pass->GetPassName();

    return command;
}

RenderCommand CreateRenderPacketCommand(RenderPacket packet, RenderPass* pass)
{
    SubmitRenderPacketCommand rpCommand;
    rpCommand.Packet = packet;

    RenderCommand command;
    command.CommandType = eRenderCommandType::eSubmitRenderPacket;
    command.Command = rpCommand;

    return command;
}

RenderCommand CreateSetRenderTargetCommand(SetRenderTargetsCommand setRTCmd, RenderPass* pass)
{
    RenderCommand command;
    command.CommandType = eRenderCommandType::eSetRenderTargets;
    command.Command = setRTCmd;

    return command;
}
}