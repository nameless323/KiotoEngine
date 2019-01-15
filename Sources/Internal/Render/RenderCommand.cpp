#include "stdafx.h"

#include "Render/RenderCommand.h"

#include "Render/RenderPass/RenderPass.h"

namespace Kioto::Renderer::RenderCommandHelpers
{
RenderCommand CreateConstantBufferCommand(const ConstantBuffer& buffer, RenderPass* pass)
{
    SubmitConstantBufferCommand cbCmd;
    cbCmd.BufferHandle = buffer.GetHandle();
    cbCmd.Index = buffer.GetIndex();
    cbCmd.Space = buffer.GetSpace();

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

Renderer::RenderCommand CreatePassEndsCommand(RenderPass* pass)
{
    RenderCommand command;
    command.CommandType = eRenderCommandType::eEndRenderPass;
    command.PassName = pass->GetPassName();

    return command;
}

}