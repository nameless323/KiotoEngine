#include "Render/ScopedGpuProfiler.h"

#include "Render/RenderCommand.h"

namespace Kioto::Renderer
{
ScopedGpuProfiler::ScopedGpuProfiler(CommandList* cmdList, std::string name)
    : mCmdList(cmdList)
{
    mCmdList->PushCommand(RenderCommandHelpers::CreateBeginGpuEventCommand(std::move(name)));
}

ScopedGpuProfiler::~ScopedGpuProfiler()
{
    mCmdList->PushCommand(RenderCommandHelpers::CreateEndGpuEventCommand());
}
}