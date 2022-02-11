#include "Render/RenderCommand.h"

#include "Render/RenderPass/RenderPass.h"
#include "Render/ConstantBuffer.h"

namespace Kioto::Renderer::RenderCommandHelpers
{
RenderCommand CreateRenderPacketCommand(RenderPacket packet, RenderPass* pass)
{
    SubmitRenderPacketCommand rpCommand;
    rpCommand.Packet = packet;

    RenderCommand command;
    command.CommandType = eRenderCommandType::eSubmitRenderPacket;
    command.Command = rpCommand;
    command.PassName = pass->GetName();

    return command;
}

RenderCommand CreateSetRenderTargetCommand(SetRenderTargetsCommand setRTCmd, RenderPass* pass)
{
    RenderCommand command;
    command.CommandType = eRenderCommandType::eSetRenderTargets;
    command.Command = setRTCmd;
    command.PassName = pass->GetName();

    return command;
}

RenderCommand CreatePassEndsCommand(RenderPass* pass)
{
    RenderCommand command;
    command.CommandType = eRenderCommandType::eEndRenderPass;
    command.PassName = pass->GetName();

    return command;
}

RenderCommand CreateBeginGpuEventCommand(std::string name)
{
    BeginGpuEventCommand cmd;
    cmd.Name = std::move(name);

    RenderCommand command;
    command.CommandType = eRenderCommandType::eBeginGpuEvent;
    command.Command = cmd;

    return command;
}

RenderCommand CreateEndGpuEventCommand()
{
    RenderCommand command;
    command.CommandType = eRenderCommandType::eEndGpuEvent;

    return command;
}

RenderCommand CreateGpuMarkerCommand(std::string name)
{
    SetGpuMarkerCommand cmd;
    cmd.Name = std::move(name);

    RenderCommand command;
    command.CommandType = eRenderCommandType::eSetGpuMarker;
    command.Command = cmd;

    return command;
}

RenderCommand CreateResourceTransitonCommand(TextureHandle handle, eResourceState destState, RenderPass* pass)
{
    ResourceTransitonCommand cmd;
    cmd.ResourceHandle = handle;
    cmd.DestState = destState;

    RenderCommand command;
    command.CommandType = eRenderCommandType::eResourceTransitonCommand;
    command.PassName = pass->GetName();
    command.Command = cmd;

    return command;
}
}