#include "Render/ScopedGpuProfiler.h"

#include "Render/RenderCommand.h"

namespace Kioto::Renderer
{
ScopedGpuProfiler::ScopedGpuProfiler(CommandList* cmdList, std::string name)
    : m_cmdList(cmdList)
{
    m_cmdList->PushCommand(RenderCommandHelpers::CreateBeginGpuEventCommand(std::move(name)));
}

ScopedGpuProfiler::~ScopedGpuProfiler()
{
    m_cmdList->PushCommand(RenderCommandHelpers::CreateEndGpuEventCommand());
}
}